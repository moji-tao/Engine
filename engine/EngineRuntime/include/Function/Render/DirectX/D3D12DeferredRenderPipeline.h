#pragma once
#include "EngineRuntime/include/Function/Render/RenderPipeline.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12RHI.h"
#include "EngineRuntime/include/Function/Render/DirectX/RenderTarget.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12RenderResource.h"
#include "EngineRuntime/include/Function/Render/DirectX/SceneCaptureCube.h"

namespace Engine
{
	class D3D12DeferredRenderPipeline final : public RenderPipeline
	{
	public:
		D3D12DeferredRenderPipeline(D3D12RHI* rhi, D3D12RenderResource* resource);

		virtual ~D3D12DeferredRenderPipeline() override;

	public:
		virtual void Render() override;

		virtual void OnResize(int width, int height) override;

		virtual void UploadResource() override;

		void CreateD3D12State();

	private:
		void CreateGlobalShaders();

		void CreateGlobalPSO();

	private:

	private:
		void SetDescriptorHeaps();

		D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;

		D3D12Resource* CurrentBackBuffer() const;

		float* CurrentBackBufferClearColor() const;

		D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

	private:
		void DepthPrePass();

		void BasePass();

		void DeferredLightingPass();

		void TAAPass();

		void PostProcessPass();

	private:
		void ResizeDepthTestBuffer();

		void ResizeGBuffers();

		void ResizeRenderTarget();

		void ResizeTAABuffer();

	private:
		D3D12RHI* mRHI = nullptr;

		D3D12RenderResource* mRenderResource = nullptr;

		std::unique_ptr<RenderTarget2D> mSceneRenderTarget = nullptr;

		uint32_t mTargetWight;

		uint32_t mTargetHeight;

		D3D12_VIEWPORT mScreenViewport;
		D3D12_RECT mScissorRect;

	private:
		//void CreateSceneCaptureCube();

		//void CreateIBLEnviromentMap();

		void CreateIBLEnviromentTexture();

	private:
		// Z-PrePass
		std::unique_ptr<Shader> mZPrePassShader = nullptr;
		GraphicsPSODescriptor mZPrePassPSODescriptor;
		D3D12TextureRef mDepthTestBuffer;

	private:
		// BasePass
		std::unique_ptr<RenderTarget2D> mPositionGBuffer;
		std::unique_ptr<RenderTarget2D> mNormalGBuffer;
		std::unique_ptr<RenderTarget2D> mColorGBuffer;
		std::unique_ptr<RenderTarget2D> mMetallicGBuffer;

		// SkyBox
		std::unique_ptr<Shader> mSkyBoxPassShader = nullptr;
		GraphicsPSODescriptor mSkyBoxPassPSODescriptor;

	private:
		// IBL
		//std::unique_ptr<Shader> mIBLEnvironmentShader = nullptr;
		//GraphicsPSODescriptor mIBLEnvironmentPSODescriptor;
		//std::unique_ptr<SceneCaptureCube> mIBLEnvironmentMap;
		//D3D12ConstantBufferRef mIBLEnviromentPassCBRef[6];
		//D3D12TextureRef mIBLEnviromentTexture = nullptr;
		D3D12TextureRef mIBLEnviromentTexture = nullptr;

	private:
		// DeferredLightingPass
		std::unique_ptr<Shader> mDeferredLightingShader = nullptr;
		GraphicsPSODescriptor mDeferredLightingPSODescriptor;
		std::unique_ptr<RenderTarget2D> mColorTexture = nullptr;

	private:
		// TAA
		std::unique_ptr<Shader> mTAAShader = nullptr;
		GraphicsPSODescriptor mTAAPSODescriptor;
		std::unique_ptr<RenderTarget2D> mVelocityBuffer = nullptr;
		D3D12TextureRef mPrevColorTexture = nullptr;
		D3D12TextureRef mCacheColorTexture = nullptr;

	private:
		// PostProcessPass
		std::unique_ptr<Shader> mPostProcessShader = nullptr;
		GraphicsPSODescriptor mPostProcessPSODescriptor;
	};
}
