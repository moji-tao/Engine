#ifdef _WIN64
#include <algorithm>
#include <sstream>

#include "EngineRuntime/include/Function/Render/DirectX/D3D12DeviceManager.h"
#include "EngineRuntime/include/Function/Window/WindowSystem.h"
#include "EngineRuntime/include/Core/Base/macro.h"
#include "nvrhi/validation.h"
#include "nvrhi/d3d12.h"

namespace Engine
{
#define HR_RETURN(hr) if(FAILED(hr)) return false;

    static bool IsNvDeviceID(UINT id)
    {
        return id == 0x10DE;
    }

    static RefCountPtr<IDXGIAdapter> FindAdapter()
    {
        RefCountPtr<IDXGIAdapter> targetAdapter;
        RefCountPtr<IDXGIFactory1> DXGIFactory;
        HRESULT hres = CreateDXGIFactory1(IID_PPV_ARGS(&DXGIFactory));
        if (hres != S_OK)
        {
            LOG_ERROR("ERROR in CreateDXGIFactory.\n"
                "For more info, get log from debug D3D runtime: (1) Install DX SDK, and enable Debug D3D from DX Control Panel Utility. (2) Install and start DbgView. (3) Try running the program again.\n");
        	return targetAdapter;
        }

        unsigned int adapterNo = 0;
        while (SUCCEEDED(hres))
        {
            RefCountPtr<IDXGIAdapter> pAdapter;
            hres = DXGIFactory->EnumAdapters(adapterNo, &pAdapter);

            if (SUCCEEDED(hres))
            {
                DXGI_ADAPTER_DESC aDesc;
                pAdapter->GetDesc(&aDesc);

                // If no name is specified, return the first adapater.  This is the same behaviour as the
                // default specified for D3D11CreateDevice when no adapter is specified.
            	targetAdapter = pAdapter;
            	break;
            }

            adapterNo++;
        }

        return targetAdapter;
    }

    static bool MoveWindowOntoAdapter(IDXGIAdapter* targetAdapter, RECT& rect)
    {
        assert(targetAdapter != NULL);

        HRESULT hres = S_OK;
        unsigned int outputNo = 0;
        while (SUCCEEDED(hres))
        {
            IDXGIOutput* pOutput = nullptr;
            hres = targetAdapter->EnumOutputs(outputNo++, &pOutput);

            if (SUCCEEDED(hres) && pOutput)
            {
                DXGI_OUTPUT_DESC OutputDesc;
                pOutput->GetDesc(&OutputDesc);
                const RECT desktop = OutputDesc.DesktopCoordinates;
                const int centreX = (int)desktop.left + (int)(desktop.right - desktop.left) / 2;
                const int centreY = (int)desktop.top + (int)(desktop.bottom - desktop.top) / 2;
                const int winW = rect.right - rect.left;
                const int winH = rect.bottom - rect.top;
                const int left = centreX - winW / 2;
                const int right = left + winW;
                const int top = centreY - winH / 2;
                const int bottom = top + winH;
                rect.left = std::max(left, (int)desktop.left);
                rect.right = std::min(right, (int)desktop.right);
                rect.bottom = std::min(bottom, (int)desktop.bottom);
                rect.top = std::max(top, (int)desktop.top);

                // If there is more than one output, go with the first found.  Multi-monitor support could go here.
                return true;
            }
        }

        return false;
    }

	bool D3D12DeviceManager::CreateDeviceAndSwapChain()
	{
		UINT windowStyle = m_DeviceParams.startFullscreen
    		? (WS_POPUP | WS_SYSMENU | WS_VISIBLE) : m_DeviceParams.startMaximized
				? (WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_MAXIMIZE)
				: (WS_OVERLAPPEDWINDOW | WS_VISIBLE);

		RECT rect = { 0, 0, LONG(m_DeviceParams.backBufferWidth), LONG(m_DeviceParams.backBufferHeight) };
        AdjustWindowRect(&rect, windowStyle, FALSE);

		RefCountPtr<IDXGIAdapter> targetAdapter;

		targetAdapter = FindAdapter();

		if (!targetAdapter)
		{
			LOG_ERROR("Could not find an adapter matching\n");
			return false;
		}

        {
            DXGI_ADAPTER_DESC aDesc;
            targetAdapter->GetDesc(&aDesc);

            std::wstring adapterName = aDesc.Description;

            // A stupid but non-deprecated and portable way of converting a wstring to a string
            std::stringstream ss;
            std::wstringstream wss;
            for (auto c : adapterName)
                ss << wss.narrow(c, '?');
            m_RendererString = ss.str();

            m_IsNvidia = IsNvDeviceID(aDesc.VendorId);
        }

        if (MoveWindowOntoAdapter(targetAdapter, rect))
        {
            glfwSetWindowPos((GLFWwindow*)WindowSystem::GetInstance()->GetWindowHandle(false), rect.left, rect.top);
        }

        RECT clientRect;
        GetClientRect((HWND)WindowSystem::GetInstance()->GetWindowHandle(true), &clientRect);
        UINT width = clientRect.right - clientRect.left;
        UINT height = clientRect.bottom - clientRect.top;

        ZeroMemory(&m_SwapChainDesc, sizeof(m_SwapChainDesc));
        m_SwapChainDesc.Width = width;
        m_SwapChainDesc.Height = height;
        m_SwapChainDesc.SampleDesc.Count = m_DeviceParams.swapChainSampleCount;
        m_SwapChainDesc.SampleDesc.Quality = 0;
        m_SwapChainDesc.BufferUsage = DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT;
        //m_SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        m_SwapChainDesc.BufferCount = m_DeviceParams.swapChainBufferCount;
        m_SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        m_SwapChainDesc.Flags = m_DeviceParams.allowModeSwitch ? DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH : 0;

        // Special processing for sRGB swap chain formats.
        // DXGI will not create a swap chain with an sRGB format, but its contents will be interpreted as sRGB.
        // So we need to use a non-sRGB format here, but store the true sRGB format for later framebuffer creation.
        switch (m_DeviceParams.swapChainFormat)  // NOLINT(clang-diagnostic-switch-enum)
        {
        case nvrhi::Format::SRGBA8_UNORM:
            m_SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case nvrhi::Format::SBGRA8_UNORM:
            m_SwapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            break;
        default:
            m_SwapChainDesc.Format = nvrhi::d3d12::convertFormat(m_DeviceParams.swapChainFormat);
            break;
        }

        if (m_DeviceParams.enableDebugRuntime)
        {
            RefCountPtr<ID3D12Debug> pDebug;
            RefCountPtr<ID3D12Debug1> pDebug1;
            HR_RETURN(D3D12GetDebugInterface(IID_PPV_ARGS(&pDebug)));
            HR_RETURN(pDebug->QueryInterface(IID_PPV_ARGS(&pDebug1)));
            pDebug1->EnableDebugLayer();
            //pDebug1->SetEnableGPUBasedValidation(true);
            
            //D3D12EnableExperimentalFeatures()
        }

        UINT dxgiFactoryFlags = m_DeviceParams.enableDebugRuntime ? DXGI_CREATE_FACTORY_DEBUG : 0;

        HR_RETURN(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_DxgiFactory)));
        BOOL supported = 0;
        if (SUCCEEDED(m_DxgiFactory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &supported, sizeof(supported))))
            m_TearingSupported = (supported != 0);

        if (m_TearingSupported)
        {
            m_SwapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
        }

        HR_RETURN(D3D12CreateDevice(targetAdapter, D3D_FEATURE_LEVEL_11_1, IID_PPV_ARGS(&m_Device12)));

        if (m_DeviceParams.enableDebugRuntime)
        {
            RefCountPtr<ID3D12InfoQueue> pInfoQueue;
            m_Device12->QueryInterface(&pInfoQueue);

            if (pInfoQueue)
            {
#ifdef _DEBUG
                pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
                pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
#endif

                D3D12_MESSAGE_ID disableMessageIDs[] = {
                    D3D12_MESSAGE_ID_CLEARDEPTHSTENCILVIEW_MISMATCHINGCLEARVALUE,
                    D3D12_MESSAGE_ID_COMMAND_LIST_STATIC_DESCRIPTOR_RESOURCE_DIMENSION_MISMATCH, // descriptor validation doesn't understand acceleration structures
                };

                D3D12_INFO_QUEUE_FILTER filter = {};
                filter.DenyList.pIDList = disableMessageIDs;
                filter.DenyList.NumIDs = sizeof(disableMessageIDs) / sizeof(disableMessageIDs[0]);
                pInfoQueue->AddStorageFilterEntries(&filter);
            }
        }

        m_DxgiAdapter = targetAdapter;

        D3D12_COMMAND_QUEUE_DESC queueDesc;
        ZeroMemory(&queueDesc, sizeof(queueDesc));
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.NodeMask = 1;
        HR_RETURN(m_Device12->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_GraphicsQueue)));
        m_GraphicsQueue->SetName(L"Graphics Queue");

        if (m_DeviceParams.enableComputeQueue)
        {
            queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
            HR_RETURN(m_Device12->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_ComputeQueue)));
        	m_ComputeQueue->SetName(L"Compute Queue");
        }

        if (m_DeviceParams.enableCopyQueue)
        {
            queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
            HR_RETURN(m_Device12->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CopyQueue)));
        	m_CopyQueue->SetName(L"Copy Queue");
        }

        m_FullScreenDesc = {};
        m_FullScreenDesc.RefreshRate.Numerator = m_DeviceParams.refreshRate;
        m_FullScreenDesc.RefreshRate.Denominator = 1;
        m_FullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
        m_FullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        m_FullScreenDesc.Windowed = !m_DeviceParams.startFullscreen;

        RefCountPtr<IDXGISwapChain1> pSwapChain1;
        HR_RETURN(m_DxgiFactory->CreateSwapChainForHwnd(m_GraphicsQueue.Get(), (HWND)WindowSystem::GetInstance()->GetWindowHandle(true), &m_SwapChainDesc, &m_FullScreenDesc, nullptr, &pSwapChain1));

        HR_RETURN(pSwapChain1->QueryInterface(IID_PPV_ARGS(&m_SwapChain)));

        nvrhi::d3d12::DeviceDesc deviceDesc;
        deviceDesc.errorCB = DefaultMessageCallback::GetInstance();
        deviceDesc.pDevice = m_Device12;
        deviceDesc.pGraphicsCommandQueue = m_GraphicsQueue;
        deviceDesc.pComputeCommandQueue = m_ComputeQueue;
        deviceDesc.pCopyCommandQueue = m_CopyQueue;

        m_NvrhiDevice = nvrhi::d3d12::createDevice(deviceDesc);

        if (m_DeviceParams.enableNvrhiValidationLayer)
        {
            m_NvrhiDevice = nvrhi::validation::createValidationLayer(m_NvrhiDevice);
        }

        if (!CreateRenderTargets())
            return false;

        HR_RETURN(m_Device12->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_FrameFence)));

        for (UINT bufferIndex = 0; bufferIndex < m_SwapChainDesc.BufferCount; bufferIndex++)
        {
            m_FrameFenceEvents.push_back(CreateEvent(nullptr, false, true, NULL));
        }

        return true;
    }

	void D3D12DeviceManager::DestroyDeviceAndSwapChain()
	{
        m_RhiSwapChainBuffers.clear();
        m_RendererString.clear();

        ReleaseRenderTargets();

        m_NvrhiDevice = nullptr;

        for (auto fenceEvent : m_FrameFenceEvents)
        {
            WaitForSingleObject(fenceEvent, INFINITE);
            CloseHandle(fenceEvent);
        }

        m_FrameFenceEvents.clear();

        if (m_SwapChain)
        {
            m_SwapChain->SetFullscreenState(false, nullptr);
        }

        m_SwapChainBuffers.clear();

        m_FrameFence = nullptr;
        m_SwapChain = nullptr;
        m_GraphicsQueue = nullptr;
        m_ComputeQueue = nullptr;
        m_CopyQueue = nullptr;
        m_Device12 = nullptr;
        m_DxgiAdapter = nullptr;
	}

	void D3D12DeviceManager::ResizeSwapChain()
	{
        ReleaseRenderTargets();

        if (!m_NvrhiDevice)
            return;

        if (!m_SwapChain)
            return;

        const HRESULT hr = m_SwapChain->ResizeBuffers(m_DeviceParams.swapChainBufferCount,
            m_DeviceParams.backBufferWidth,
            m_DeviceParams.backBufferHeight,
            m_SwapChainDesc.Format,
            m_SwapChainDesc.Flags);

        if (FAILED(hr))
        {
            LOG_FATAL("ResizeBuffers failed");
        }

        bool ret = CreateRenderTargets();
        if (!ret)
        {
            LOG_FATAL("CreateRenderTarget failed");
        }
    }

	void D3D12DeviceManager::BeginFrame()
	{
        DXGI_SWAP_CHAIN_DESC1 newSwapChainDesc;
        DXGI_SWAP_CHAIN_FULLSCREEN_DESC newFullScreenDesc;
        if (SUCCEEDED(m_SwapChain->GetDesc1(&newSwapChainDesc)) && SUCCEEDED(m_SwapChain->GetFullscreenDesc(&newFullScreenDesc)))
        {
            if (m_FullScreenDesc.Windowed != newFullScreenDesc.Windowed)
            {
                BackBufferResizing();

                m_FullScreenDesc = newFullScreenDesc;
                m_SwapChainDesc = newSwapChainDesc;
                m_DeviceParams.backBufferWidth = newSwapChainDesc.Width;
                m_DeviceParams.backBufferHeight = newSwapChainDesc.Height;

                if (newFullScreenDesc.Windowed)
                    WindowSystem::GetInstance()->SetWindowScreenDisplay(newSwapChainDesc.Width, newSwapChainDesc.Height);
                
                ResizeSwapChain();
                BackBufferResized();
            }
        }

        auto bufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

        WaitForSingleObject(m_FrameFenceEvents[bufferIndex], INFINITE);

	}

	void D3D12DeviceManager::Present()
	{
        if (!m_windowVisible)
            return;

        auto bufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

        UINT presentFlags = 0;
        if (!m_DeviceParams.vsyncEnabled && m_FullScreenDesc.Windowed && m_TearingSupported)
            presentFlags |= DXGI_PRESENT_ALLOW_TEARING;

        m_SwapChain->Present(m_DeviceParams.vsyncEnabled ? 1 : 0, presentFlags);

        m_FrameFence->SetEventOnCompletion(m_FrameCount, m_FrameFenceEvents[bufferIndex]);
        m_GraphicsQueue->Signal(m_FrameFence, m_FrameCount);
        m_FrameCount++;
    }

	nvrhi::IDevice* D3D12DeviceManager::GetDevice() const
	{
		return m_NvrhiDevice;
	}

    const char* D3D12DeviceManager::GetRendererString() const
    {
        return m_RendererString.c_str();
    }

    nvrhi::GraphicsAPI D3D12DeviceManager::GetGraphicsAPI() const
    {
        return nvrhi::GraphicsAPI::D3D12;
    }

    nvrhi::ITexture* D3D12DeviceManager::GetCurrentBackBuffer()
    {
        return m_RhiSwapChainBuffers[m_SwapChain->GetCurrentBackBufferIndex()];
    }

    nvrhi::ITexture* D3D12DeviceManager::GetBackBuffer(uint32_t index)
    {
        if (index < m_RhiSwapChainBuffers.size())
            return m_RhiSwapChainBuffers[index];
        return nullptr;
    }

    uint32_t D3D12DeviceManager::GetCurrentBackBufferIndex()
    {
        return m_SwapChain->GetCurrentBackBufferIndex();
    }

    uint32_t D3D12DeviceManager::GetBackBufferCount()
    {
        return m_SwapChainDesc.BufferCount;
    }

    bool D3D12DeviceManager::CreateRenderTargets()
    {
        m_SwapChainBuffers.resize(m_SwapChainDesc.BufferCount);
        m_RhiSwapChainBuffers.resize(m_SwapChainDesc.BufferCount);

        for (UINT n = 0; n < m_SwapChainDesc.BufferCount; n++)
        {
            const HRESULT hr = m_SwapChain->GetBuffer(n, IID_PPV_ARGS(&m_SwapChainBuffers[n]));
            HR_RETURN(hr);

        	nvrhi::TextureDesc textureDesc;
            textureDesc.width = m_DeviceParams.backBufferWidth;
            textureDesc.height = m_DeviceParams.backBufferHeight;
            textureDesc.sampleCount = m_DeviceParams.swapChainSampleCount;
            textureDesc.sampleQuality = m_DeviceParams.swapChainSampleQuality;
            textureDesc.format = m_DeviceParams.swapChainFormat;
            textureDesc.debugName = "SwapChainBuffer";
            textureDesc.isRenderTarget = true;
            textureDesc.isUAV = false;
            textureDesc.initialState = nvrhi::ResourceStates::Present;
            textureDesc.keepInitialState = true;

            m_RhiSwapChainBuffers[n] = m_NvrhiDevice->createHandleForNativeTexture(nvrhi::ObjectTypes::D3D12_Resource, nvrhi::Object(m_SwapChainBuffers[n]), textureDesc);
        }

        return true;
    }

    void D3D12DeviceManager::ReleaseRenderTargets()
    {
        // Make sure that all frames have finished rendering
        m_NvrhiDevice->waitForIdle();

        // Release all in-flight references to the render targets
        m_NvrhiDevice->runGarbageCollection();

        // Set the events so that WaitForSingleObject in OneFrame will not hang later
        for (auto e : m_FrameFenceEvents)
            SetEvent(e);

        // Release the old buffers because ResizeBuffers requires that
        m_RhiSwapChainBuffers.clear();
        m_SwapChainBuffers.clear();
    }

}

#endif