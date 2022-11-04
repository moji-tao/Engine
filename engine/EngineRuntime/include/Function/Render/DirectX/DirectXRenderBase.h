#pragma once
#ifdef _WIN64
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include "EngineRuntime/include/Function/Render/RHI.h"

using namespace Microsoft::WRL;

namespace Engine
{
	class DirectXRenderBase : public RHI
	{
	public:
		DirectXRenderBase() = default;

		virtual ~DirectXRenderBase() override;

	public:
		virtual bool Initialize(InitConfig* info) override;

		virtual bool Tick(float deltaTime) override = 0;

		virtual void Finalize() override = 0;

		virtual void ResizeEngineContentViewport(float offsetX, float offsetY, float width, float height) override;

		virtual void OnResize();

	public:
		float AspectRatio()const;

	protected:
		void FlushCommandQueue();

#ifdef _DEBUG
		void LogAdapters();
#endif

		void CreateCommandObjects();

		void CreateSwapChain(WindowSystem& window);

		void CreateHeaps();

	protected:
		ID3D12Resource* CurrentBackBuffer()const;

		D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;

		D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;

	protected:
		int mClientWidth;
		int mClientHeight;

		UINT mRTVDescriptorSize = 0;
		UINT mDSVDescriptorSize = 0;
		UINT mCBV_SRV_UAVDescriptorSize = 0;

		bool m4xMsaaState = false;
		UINT m4xMsaaQuality;

		const static UINT mSwapChainBufferCount = 2u;
		ComPtr<IDXGIFactory5> mDXGIFactory;
		ComPtr<ID3D12Device4> mD3DDevice;
		ComPtr<ID3D12CommandAllocator> mCMDListAlloc;
		ComPtr<ID3D12GraphicsCommandList> mCommandList;
		ComPtr<ID3D12CommandQueue> mCommandQueue;
		ComPtr<IDXGISwapChain3> mSwapChain;
		ComPtr<ID3D12DescriptorHeap> mRTVHeap;
		ComPtr<ID3D12DescriptorHeap> mDSVHeap;
		ComPtr<ID3D12Resource> mSwapChainBuffer[mSwapChainBufferCount];
		ComPtr<ID3D12Resource> mDepthStencilBuffer;
		ComPtr<ID3D12Fence>	mFence;

		int mCurrBackBufferIndex = 0;
		UINT64 mCurrentFence;

		D3D12_VIEWPORT mScreenViewport;
		D3D12_RECT mScissorRect;

		DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	};
}
#endif