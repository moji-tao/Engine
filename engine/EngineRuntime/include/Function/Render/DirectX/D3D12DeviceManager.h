#pragma once
#ifdef _WIN64
#include <d3d12.h>
#include <dxgi1_6.h>
#include "EngineRuntime/include/Function/Render/DeviceManager.h"

namespace Engine
{
    using nvrhi::RefCountPtr;

	class D3D12DeviceManager : public DeviceManager
	{
	public:
		D3D12DeviceManager() = default;

		virtual ~D3D12DeviceManager() override = default;

	private:
        virtual bool CreateDeviceAndSwapChain() override;

        virtual void DestroyDeviceAndSwapChain() override;

        virtual void ResizeSwapChain() override;

        virtual void BeginFrame() override;

        virtual void Present() override;

	public:
        virtual nvrhi::IDevice* GetDevice() const override;

        virtual const char* GetRendererString() const override;

        virtual nvrhi::GraphicsAPI GetGraphicsAPI() const override;

        virtual nvrhi::ITexture* GetCurrentBackBuffer() override;

        virtual nvrhi::ITexture* GetBackBuffer(uint32_t index) override;

        virtual uint32_t GetCurrentBackBufferIndex() override;

        virtual uint32_t GetBackBufferCount() override;

	private:
        bool CreateRenderTargets();

        void ReleaseRenderTargets();

	protected:
        RefCountPtr<ID3D12Device4>                   m_Device12;
        RefCountPtr<ID3D12CommandQueue>             m_GraphicsQueue;
        RefCountPtr<ID3D12CommandQueue>             m_ComputeQueue;
        RefCountPtr<ID3D12CommandQueue>             m_CopyQueue;
        RefCountPtr<IDXGISwapChain3>                m_SwapChain;
        DXGI_SWAP_CHAIN_DESC1                       m_SwapChainDesc{};
        DXGI_SWAP_CHAIN_FULLSCREEN_DESC             m_FullScreenDesc{};
        RefCountPtr<IDXGIAdapter1>                   m_DxgiAdapter;
        bool                                        m_TearingSupported = false;

        std::vector<RefCountPtr<ID3D12Resource>>    m_SwapChainBuffers;
        std::vector<nvrhi::TextureHandle>           m_RhiSwapChainBuffers;
        RefCountPtr<ID3D12Fence>                    m_FrameFence;
        std::vector<HANDLE>                         m_FrameFenceEvents;

        UINT64                                      m_FrameCount = 1;

        nvrhi::DeviceHandle                         m_NvrhiDevice;

        std::string                                 m_RendererString;
	};
}
#endif
