#pragma once
#ifdef _WIN64
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include "EngineRuntime/include/Function/Render/RHI.h"
#include "EngineRuntime/include/Function/Render/DirectX/FrameResource.h"
using namespace Microsoft::WRL;

namespace Engine
{
	class D3D12RHI : public RHI
	{
	public:
		D3D12RHI() = default;

		virtual ~D3D12RHI() override = default;

	public:
		virtual bool Initialize(InitConfig* info) override;

		virtual void Finalize() override;

		virtual void ResizeEngineContentViewport(float offsetX, float offsetY, float width, float height) override;

	public:
		virtual void CommandListReset() override;

		virtual void CommandListClose() override;

		virtual void ExecuteCommandLists() override;

		virtual void WaitForFences() override;

		virtual void ResizeWindow() override;

		virtual void SubmitRendering() override;

		virtual void ResetCommandAllocator() override;

		virtual void UploadVertexData(void* data, unsigned long long dataLength) override;

		virtual void UploadIndexData(void* data, unsigned long long dataLength) override;

		virtual void UploadMainPassData(const PassConstants& passComstants) override;

		virtual float GetAspect() override;

		virtual void PrepareContext() override;

	private:
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

	protected:
		void BuildRootSignature();

		void BuildShadersAndInputLayout();

		void BuildFrameResources(unsigned int renderItemCount);

		void BuildDescriptorHeaps(unsigned int renderItemCount);

		void BuildConstantBufferViews(unsigned int renderItemCount);

		void BuildPSOs();

	protected:
		ComPtr<ID3D12DescriptorHeap> mCBVHeap = nullptr;
		ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
		ComPtr<ID3DBlob> mVSByteCode;
		ComPtr<ID3DBlob> mPSByteCode;
		TArray<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
		static const int gNumFrameResources;
		int mCurrentFrameResourceIndex = 0;
		TArray<std::unique_ptr<FrameResource>> mFrameResources;
		UINT mPassCbvOffset;
		ComPtr<ID3D12PipelineState> mPSO;

		ComPtr<ID3D12Resource> mVertexBufferGPU;
		ComPtr<ID3D12Resource> mVertexBufferUpload;
		ComPtr<ID3D12Resource> mIndexBufferGPU;
		ComPtr<ID3D12Resource> mIndexBufferUpload;
	};
}
#endif