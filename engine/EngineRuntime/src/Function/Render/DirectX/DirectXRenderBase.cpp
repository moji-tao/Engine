#ifdef _WIN64
#ifdef _DEBUG
#include <dxgidebug.h>
#endif
#include <d3dcompiler.h>
#include "EngineRuntime/include/Function/Render/DirectX/d3dx12.h"
#include "EngineRuntime/include/Function/Render/DirectX/DirectXRenderBase.h"
#include "EngineRuntime/include/Function/Render/DirectX/DirectXUtil.h"
#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Platform/CharSet.h"

namespace Engine
{
	DirectXRenderBase::~DirectXRenderBase()
	{

	}

	float DirectXRenderBase::AspectRatio() const
	{
		return static_cast<float>(mClientWidth) / mClientHeight;
	}


	bool DirectXRenderBase::Initialize(InitConfig* info)
	{
		mRHIType = RHIType::D3D12;

		WindowSystem* pWindow = (WindowSystem*)info->windowHandle;

		assert(!pWindow->ShouldClose());

		pWindow->RegisterWindowResizeCallback([this](int Width, int Height)
			{
				if (Width == 0 && Height == 0)
					return;
				this->mClientWidth = Width;
				this->mClientHeight = Height;
				this->OnResize();
				//this->Tick();
			});

		mClientWidth = pWindow->GetWindowWidth();
		mClientHeight = pWindow->GetWindowHeight();

		UINT nDXGIFactoryFlags = 0U;
		// 打开显示子系统的调试支持
#if defined(_DEBUG)
		ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)), "Debug层创建失败");
		debugController->EnableDebugLayer();
		nDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

		// 创建DXGI Factory对象
		ThrowIfFailed(CreateDXGIFactory2(nDXGIFactoryFlags, IID_PPV_ARGS(&mDXGIFactory)), "DXGI Factory对象 创建失败");

		HRESULT hardwareResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&mD3DDevice));
		if (FAILED(hardwareResult))
		{
			ComPtr<IDXGIAdapter> pWarpAdapter;
			ThrowIfFailed(mDXGIFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)), "没有找到图形设备");

			ThrowIfFailed(D3D12CreateDevice(pWarpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&mD3DDevice)), "D3D12驱动创建失败");
		}

		ThrowIfFailed(mD3DDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)), "围栏创建失败");

		mRTVDescriptorSize = mD3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		mDSVDescriptorSize = mD3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		mCBV_SRV_UAVDescriptorSize = mD3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
		msQualityLevels.Format = mBackBufferFormat;
		msQualityLevels.SampleCount = 4;
		msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		msQualityLevels.NumQualityLevels = 0;
		ThrowIfFailed(mD3DDevice->CheckFeatureSupport(
			D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
			&msQualityLevels,
			sizeof(msQualityLevels)), "检测到不支持4x MSAA");

		m4xMsaaQuality = msQualityLevels.NumQualityLevels;
		assert(m4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

#ifdef _DEBUG
		LogAdapters();
#endif
		CreateCommandObjects();
		CreateSwapChain(*pWindow);
		CreateHeaps();

		OnResize();

		ResizeEngineContentViewport(0, 0, mClientWidth, mClientHeight);

		return true;
	}

#ifdef _DEBUG
	void DirectXRenderBase::LogAdapters()
	{
		// 枚举适配器
		ComPtr<IDXGIAdapter1> pIAdapter1;
		DXGI_ADAPTER_DESC1 stAdapterDesc = {};
		D3D_FEATURE_LEVEL emFeatureLevel = D3D_FEATURE_LEVEL_12_1;
		for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != mDXGIFactory->EnumAdapters1(adapterIndex, &pIAdapter1); ++adapterIndex)
		{
			pIAdapter1->GetDesc1(&stAdapterDesc);

			if (stAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				//跳过软件虚拟适配器设备
				continue;
			}

			//检查适配器对D3D支持的兼容级别，这里直接要求支持12.1的能力，注意返回接口的那个参数被置为了nullptr，这样
			//就不会实际创建一个设备了，也不用我们啰嗦的再调用release来释放接口。这也是一个重要的技巧，请记住！
			if (SUCCEEDED(D3D12CreateDevice(pIAdapter1.Get(), emFeatureLevel, _uuidof(ID3D12Device), nullptr)))
			{
				char pszLog[MAX_PATH];
				memset(pszLog, 0x00, MAX_PATH);
				UnicodeToAnsi(stAdapterDesc.Description, pszLog);
				LOG_INFO("检测到显卡 {0}", pszLog);
			}
		}
	}
#endif

	void DirectXRenderBase::CreateCommandObjects()
	{
		// 创建直接命令队列
		D3D12_COMMAND_QUEUE_DESC stQueueDesc = {};
		stQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		stQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		ThrowIfFailed(mD3DDevice->CreateCommandQueue(&stQueueDesc, IID_PPV_ARGS(&mCommandQueue)), "命令队列创建失败");

		// 创建命令列表分配器
		ThrowIfFailed(mD3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mCMDListAlloc.GetAddressOf())), "创建命令列表分配器失败");

		// 创建图形命令列表
		ThrowIfFailed(mD3DDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCMDListAlloc.Get(), nullptr, IID_PPV_ARGS(mCommandList.GetAddressOf())), "创建图形命令列表失败");

		ThrowIfFailed(mCommandList->Close(), "命令队列关闭失败");
	}

	void DirectXRenderBase::CreateSwapChain(WindowSystem& window)
	{
		mSwapChain.Reset();

		// 创建交换链
		DXGI_SWAP_CHAIN_DESC1 stSwapChainDesc = {};
		stSwapChainDesc.BufferCount = mSwapChainBufferCount;
		stSwapChainDesc.Width = mClientWidth;
		stSwapChainDesc.Height = mClientHeight;
		stSwapChainDesc.Format = mBackBufferFormat;
		stSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		stSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		stSwapChainDesc.SampleDesc.Quality = m4xMsaaState ? m4xMsaaQuality - 1 : 0;
		stSwapChainDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
		ComPtr<IDXGISwapChain1> pISwapChain1;

		ThrowIfFailed(mDXGIFactory->CreateSwapChainForHwnd(
			mCommandQueue.Get(),		// 交换链需要命令队列，Present命令要执行
			(HWND)window.GetWindowHandle(true),
			&stSwapChainDesc,
			nullptr,
			nullptr,
			pISwapChain1.GetAddressOf()
		), "交换链创建失败1");

		ThrowIfFailed(pISwapChain1.As(&mSwapChain), "交换链创建失败2");

		//得到当前后缓冲区的序号，也就是下一个将要呈送显示的缓冲区的序号
		//注意此处使用了高版本的SwapChain接口的函数
		mCurrBackBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
	}

	void DirectXRenderBase::CreateHeaps()
	{
		//创建RTV(渲染目标视图)描述符堆(这里堆的含义应当理解为数组或者固定大小元素的固定大小显存池)
		D3D12_DESCRIPTOR_HEAP_DESC stRTVHeapDesc = {};
		stRTVHeapDesc.NumDescriptors = mSwapChainBufferCount;
		stRTVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		stRTVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(mD3DDevice->CreateDescriptorHeap(&stRTVHeapDesc, IID_PPV_ARGS(mRTVHeap.GetAddressOf())), "创建RTV(渲染目标视图)描述符堆失败");

		D3D12_DESCRIPTOR_HEAP_DESC stDSVHeapDesc = {};
		stDSVHeapDesc.NumDescriptors = 1;
		stDSVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		stDSVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(mD3DDevice->CreateDescriptorHeap(&stDSVHeapDesc, IID_PPV_ARGS(mDSVHeap.GetAddressOf())), "创建DSV(模板、深度测试目标视图)描述符失败");
	}

	void DirectXRenderBase::OnResize()
	{
		assert(mD3DDevice);
		assert(mSwapChain);
		assert(mCMDListAlloc);

		FlushCommandQueue();

		mCMDListAlloc->Reset();
		ThrowIfFailed(mCommandList->Reset(mCMDListAlloc.Get(), nullptr), "重置命令列表失败");

		for (int i = 0; i < mSwapChainBufferCount; ++i)
		{
			mSwapChainBuffer[i].Reset();
		}
		mDepthStencilBuffer.Reset();

		ThrowIfFailed(mSwapChain->ResizeBuffers(mSwapChainBufferCount, mClientWidth, mClientHeight,
			mBackBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH), "重置交换链缓冲区失败");

		mCurrBackBufferIndex = 0;

		//创建渲染目标视图
		D3D12_CPU_DESCRIPTOR_HANDLE stRTVHandle = mRTVHeap->GetCPUDescriptorHandleForHeapStart();
		for (UINT i = 0; i < mSwapChainBufferCount; i++)
		{
			ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i])), "创建渲染目标视图 失败");

			mD3DDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, stRTVHandle);

			stRTVHandle.ptr += mRTVDescriptorSize;
		}

		//创建深度/模板缓冲区及其视图
		D3D12_RESOURCE_DESC depthStencilDesc = {};
		depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthStencilDesc.Alignment = 0;
		depthStencilDesc.Width = mClientWidth;
		depthStencilDesc.Height = mClientHeight;
		depthStencilDesc.DepthOrArraySize = 1;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		depthStencilDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
		depthStencilDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
		depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		D3D12_CLEAR_VALUE optClear = {};
		optClear.Format = mDepthStencilFormat;
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;
		CD3DX12_HEAP_PROPERTIES stHeapProp(D3D12_HEAP_TYPE_DEFAULT);
		ThrowIfFailed(mD3DDevice->CreateCommittedResource(&stHeapProp, D3D12_HEAP_FLAG_NONE,
			&depthStencilDesc, D3D12_RESOURCE_STATE_COMMON, &optClear, IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())), "创建深度/模板缓冲区及其视图 失败");

		// 利用此资源的格式，为整个资源的第0个mip层创建描述符
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = mDepthStencilFormat;
		dsvDesc.Texture2D.MipSlice = 0;
		mD3DDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), &dsvDesc, DepthStencilView());

		// 将资源从初始状态转换为深度缓冲区
		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

		ThrowIfFailed(mCommandList->Close(), "命令列表关闭失败");
		ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		FlushCommandQueue();

		mScissorRect.left = 0;
		mScissorRect.top = 0;
		mScissorRect.right = mClientWidth;
		mScissorRect.bottom = mClientHeight;
	}

	void DirectXRenderBase::FlushCommandQueue()
	{
		// 增加围栏值，接下来将命令标记到此围栏点
		++mCurrentFence;

		// 向命令队列中添加一条用来设置新围栏的命令
		// 由于这条命令要交由GPU处理，所以在GPU处理完命令队列中此Signal()的所有命令之前，它并不会设置新的围栏点
		ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), mCurrentFence), "设置围栏失败1");

		// 在CPU端等待GPU，直到后者执行完这个围栏点之前的所有命令
		if (mFence->GetCompletedValue() < mCurrentFence)
		{
			HANDLE eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);

			// 若GPU命中当前的围栏(即执行到Signal()指令，修改了围栏值)，则激发预定事件
			ThrowIfFailed(mFence->SetEventOnCompletion(mCurrentFence, eventHandle), "设置围栏失败2");

			// 等待GPU命中围栏，激发事件
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}

	void DirectXRenderBase::ResizeEngineContentViewport(float offsetX, float offsetY, float width, float height)
	{
		mScreenViewport.TopLeftX = offsetX;
		mScreenViewport.TopLeftY = offsetY;
		mScreenViewport.Width = width;
		mScreenViewport.Height = height;
		mScreenViewport.MinDepth = 0.0f;
		mScreenViewport.MaxDepth = 1.0f;
	}

	ID3D12Resource* DirectXRenderBase::CurrentBackBuffer() const
	{
		return mSwapChainBuffer[mCurrBackBufferIndex].Get();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DirectXRenderBase::CurrentBackBufferView() const
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(mRTVHeap->GetCPUDescriptorHandleForHeapStart(), mCurrBackBufferIndex, mRTVDescriptorSize);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DirectXRenderBase::DepthStencilView()const
	{
		return mDSVHeap->GetCPUDescriptorHandleForHeapStart();
	}
}

#endif