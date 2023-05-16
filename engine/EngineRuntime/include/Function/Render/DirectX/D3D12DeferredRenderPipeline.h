#pragma once
#include "EngineRuntime/include/Function/Render/RenderPipeline.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12RHI.h"
#include "EngineRuntime/include/Function/Render/DirectX/RenderTarget.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12RenderResource.h"
#include "EngineRuntime/include/Function/Render/DirectX/SceneCaptureCube.h"
#include "EngineRuntime/include/Function/Render/DirectX/ShadowMap.h"

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
		void ShadowsPass();

		void DepthPrePass();

		void BasePass();

		void SSAOPass();

		void DeferredLightingPass();

		void SSRPass();

		void TAAPass();

		void PostProcessPass();

	private:
		void ResizeDepthTestBuffer();

		void ResizeGBuffers();

		void ResizeRenderTarget();

		void ResizeSSAOBuffer();

		void ResizeTAABuffer();

	private:
		void DrawCallScreen(ID3D12GraphicsCommandList* commandList);

		void GenerateDirectionalShadowMap(const ShadowParameter& shadowParameter);

		void GenerateOmnidirectionalShadowMap(ShadowParameter& shadowParameter);

	private:
		D3D12RHI* mRHI = nullptr;

		D3D12RenderResource* mRenderResource = nullptr;

		std::unique_ptr<RenderTarget2D> mSceneRenderTarget = nullptr;

		uint32_t mTargetWight;

		uint32_t mTargetHeight;

		D3D12_VIEWPORT mScreenViewport;
		D3D12_RECT mScissorRect;

	private:
		void CreateSceneCaptureCube();

		// 环境贴图
		void CreateIBLEnvironmentMap();

		// 环境光漫反射部分
		void CreateIBLIrradianceMap();

		// 环境光镜面反射部分
		void CreateIBLPrefilterEnvironmentMap();

		//void CreateIBLEnviromentTexture();

	private:
		// Z-PrePass
		std::unique_ptr<Shader> mZPrePassShader = nullptr;
		GraphicsPSODescriptor mZPrePassPSODescriptor;
		D3D12TextureRef mDepthTestBuffer;

	private:
		// ShadowMap
		std::unique_ptr<Shader> mOmnidirectionalShadowMapShader = nullptr;
		GraphicsPSODescriptor mOmnidirectionalShadowMapPSODescriptor;
		std::unique_ptr<Shader> mSingleShadowMapShader = nullptr;
		GraphicsPSODescriptor mSingleShadowMapPSODescriptor;
		std::unique_ptr<ShadowMap2D> mShadowMap2D[MAX_2DSHADOWMAP];
		std::unique_ptr<ShadowMapCube> mShadowMapCube[MAX_CubeSHADOWMAP];
		uint32_t mCurrentShadowMap2DIndex;
		uint32_t mCurrentShadowMapCubeIndex;
		D3D12ConstantBufferRef ShadowPassCBRef = nullptr;

	private:
		// BasePass
		std::unique_ptr<RenderTarget2D> mPositionGBuffer;
		std::unique_ptr<RenderTarget2D> mNormalGBuffer;
		std::unique_ptr<RenderTarget2D> mBaseColorGBuffer;
		std::unique_ptr<RenderTarget2D> mMetallicRoughnessGBuffer;
		std::unique_ptr<RenderTarget2D> mEmissiveGBuffer;

		// SkyBox
		std::unique_ptr<Shader> mSkyBoxPassShader = nullptr;
		GraphicsPSODescriptor mSkyBoxPassPSODescriptor;

	private:
		// IBL
		std::unique_ptr<Shader> mIBLEnvironmentShader = nullptr;
		GraphicsPSODescriptor mIBLEnvironmentPSODescriptor;
		std::unique_ptr<SceneCaptureCube> mIBLEnvironmentMap;
		D3D12ConstantBufferRef mIBLEnvironmentPassCBRef[6];
		D3D12TextureRef mIBLEquirectangularTexture = nullptr;

		std::unique_ptr<Shader> mIBLIrradianceShader = nullptr;
		GraphicsPSODescriptor mIBLIrradiancePSODescriptor;
		std::unique_ptr<SceneCaptureCube> mIBLIrradianceMap;
		D3D12ConstantBufferRef mIBLIrradiancePassCBRef[6];

		static const uint32_t mIBLPrefilterMaxMipLevel = 5;
		std::unique_ptr<Shader> mIBLPrefilterShader = nullptr;
		GraphicsPSODescriptor mIBLPrefilterPSODescriptor;
		std::vector<std::unique_ptr<SceneCaptureCube>> mIBLPrefilterMap;
		D3D12ConstantBufferRef mIBLPrefilterPassCBRef[6 * mIBLPrefilterMaxMipLevel];

		D3D12TextureRef mBRDFLUTTexture = nullptr;

		//D3D12TextureRef mIBLEnviromentTexture = nullptr;

	private:
		// SSAO
		std::unique_ptr<Shader> mSSAOShader = nullptr;
		GraphicsPSODescriptor mSSAOPSODescriptor;
		std::unique_ptr<RenderTarget2D> mSSAOTexture = nullptr;

		std::unique_ptr<Shader> mVertBlurShader = nullptr;
		ComputePSODescriptor mVertBlurPSODescriptor;
		std::unique_ptr<Shader> mHorzBlurShader = nullptr;
		ComputePSODescriptor mHorzBlurPSODescriptor;
		D3D12TextureRef mBlur1SSAOTexture = nullptr;
		D3D12TextureRef mBlur2SSAOTexture = nullptr;
		D3D12ConstantBufferRef mSSAOBlurCBRef = nullptr;

	private:
		// DeferredLightingPass
		std::unique_ptr<Shader> mDeferredLightingShader = nullptr;
		GraphicsPSODescriptor mDeferredLightingPSODescriptor;
		std::unique_ptr<RenderTarget2D> mColorTexture = nullptr;

	private:
		std::unique_ptr<Shader> mSSRShader = nullptr;
		GraphicsPSODescriptor mSSRPSODescriptor;

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

	struct EnvironmentConstant
	{
		Matrix4x4 View = Matrix4x4::IDENTITY;
		Matrix4x4 Proj = Matrix4x4::IDENTITY;
		float Roughness;
	};
}
