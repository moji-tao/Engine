#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Core/Math/Color.h"
#include "EngineRuntime/include/Function/Render/RenderSystem.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12RHI.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12DeferredRenderPipeline.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12Shader.h"
#include "EngineRuntime/include/Resource/AssetManager/AssetManager.h"

namespace Engine
{
	std::vector<float> CalcGaussWeights(float sigma)
	{
		float twoSigma2 = 2.0f * sigma * sigma;

		// Estimate the blur radius based on sigma since sigma controls the "width" of the bell curve.
		// For example, for sigma = 3, the width of the bell curve is 
		int blurRadius = (int)ceil(2.0f * sigma);

		const int MaxBlurRadius = 5;
		assert(blurRadius <= MaxBlurRadius);

		std::vector<float> weights;
		weights.resize(2 * blurRadius + 1);

		float weightSum = 0.0f;

		for (int i = -blurRadius; i <= blurRadius; ++i)
		{
			float x = (float)i;

			weights[i + blurRadius] = expf(-x * x / twoSigma2);

			weightSum += weights[i + blurRadius];
		}

		// Divide by the sum so all the weights add up to 1.0.
		for (int i = 0; i < weights.size(); ++i)
		{
			weights[i] /= weightSum;
		}

		return weights;
	}

	D3D12DeferredRenderPipeline::D3D12DeferredRenderPipeline(D3D12RHI* rhi, D3D12RenderResource* resource)
		:mRHI(rhi), mRenderResource(resource), mCurrentShadowMap2DIndex(0), mCurrentShadowMapCubeIndex(0)
	{
		D3D12_VIEWPORT ScreenViewport;
		D3D12_RECT ScissorRect;
		mRHI->GetViewport()->GetD3DViewport(ScreenViewport, ScissorRect);

		OnResize((int)ScreenViewport.Width, (int)ScreenViewport.Height);

		CreateD3D12State();
	}

	D3D12DeferredRenderPipeline::~D3D12DeferredRenderPipeline()
	{ }

	void D3D12DeferredRenderPipeline::Render()
	{
		mRHI->ResetCommandAllocator();
		mRHI->ResetCommandList();

		if (RenderSystem::GetInstance()->rdoc_api != nullptr && RenderSystem::GetInstance()->mFrameCount == 0)
		{
			RenderSystem::GetInstance()->rdoc_api->StartFrameCapture(NULL, NULL);
		}

		if (mImGuiDevice != nullptr)
		{
			mImGuiDevice->DrawUI();
		}

		SetDescriptorHeaps();

		if (RenderSystem::GetInstance()->mFrameCount == 0)
		{
			CreateIBLEnvironmentMap();

			CreateIBLIrradianceMap();

			CreateIBLPrefilterEnvironmentMap();
		}

		UploadResource();

		ShadowsPass();

		DepthPrePass();

		BasePass();

		SSAOPass();

		DeferredLightingPass();

		if (RenderSystem::GetInstance()->mEnableTAA)
		{
			TAAPass();
		}

		PostProcessPass();

		mRHI->ExecuteCommandLists();

		if (mImGuiDevice != nullptr)
		{
			mImGuiDevice->EndDraw();
		}

		mRHI->Present();
		mRHI->FlushCommandQueue();
		mRHI->EndFrame();

		if (RenderSystem::GetInstance()->rdoc_api != nullptr && RenderSystem::GetInstance()->mFrameCount == 0)
		{
			RenderSystem::GetInstance()->rdoc_api->EndFrameCapture(NULL, NULL);
		}
	}

	void D3D12DeferredRenderPipeline::OnResize(int width, int height)
	{
		if (mImGuiDevice == nullptr)
			return;

		ASSERT(width > 0 && height > 0);

		mTargetWight = (uint32_t)width;

		mTargetHeight = (uint32_t)height;

		mScreenViewport.MinDepth = 0;
		mScreenViewport.MaxDepth = 1;
		mScreenViewport.Width = mTargetWight;
		mScreenViewport.Height = mTargetHeight;
		mScreenViewport.TopLeftX = 0;
		mScreenViewport.TopLeftY = 0;
		mScissorRect.left = 0;
		mScissorRect.top = 0;
		mScissorRect.right = mTargetWight;
		mScissorRect.bottom = mTargetHeight;

		ResizeDepthTestBuffer();

		ResizeGBuffers();

		ResizeRenderTarget();

		ResizeSSAOBuffer();

		ResizeTAABuffer();
	}

	void D3D12DeferredRenderPipeline::UploadResource()
	{
		mRenderResource->UpdateRenderResource();
	}

	void D3D12DeferredRenderPipeline::CreateD3D12State()
	{
		mRenderResource->UploadDefaultVertexResource();

		CreateSceneCaptureCube();
		//CreateIBLEnviromentTexture();
		CreateGlobalShaders();
		CreateGlobalPSO();

		/*
		for (int i = 0; i < MAX_2DSHADOWMAP; ++i)
		{
			mShadowMap2D[i] = std::make_unique<ShadowMap2D>(SHADOWMAP2D_SIZE, mRHI);
		}

		for (int i = 0; i < MAX_CubeSHADOWMAP; ++i)
		{
			mShadowMapCube[i] = std::make_unique<ShadowMapCube>(SHADOWMAPCUBE_SIZE, mRHI);
		}
		*/
	}

	void D3D12DeferredRenderPipeline::CreateGlobalShaders()
	{
		{
			ShaderInfo shaderInfo;
			std::shared_ptr<Blob> vsBlob = EngineFileSystem::GetInstance()->ReadFile("Shaders/Z-PrePassVS.cso");
			std::shared_ptr<Blob> psBlob = EngineFileSystem::GetInstance()->ReadFile("Shaders/Z-PrePassPS.cso");
			if (vsBlob == nullptr || psBlob == nullptr)
			{
				LOG_FATAL("Z-PrePassShader文件读取出错");
			}
			shaderInfo.mVertexShader = vsBlob;
			shaderInfo.mPixelShader = psBlob;
			mZPrePassShader = std::make_unique<Shader>(shaderInfo, mRHI);
		}

		{
			ShaderInfo shaderInfo;
			std::shared_ptr<Blob> vsBlob = EngineFileSystem::GetInstance()->ReadFile("Shaders/OmnidirectionalShadowMapVS.cso");
			std::shared_ptr<Blob> gsBlob = EngineFileSystem::GetInstance()->ReadFile("Shaders/OmnidirectionalShadowMapGS.cso");
			std::shared_ptr<Blob> psBlob = EngineFileSystem::GetInstance()->ReadFile("Shaders/OmnidirectionalShadowMapPS.cso");
			if (vsBlob == nullptr || psBlob == nullptr || gsBlob == nullptr)
			{
				LOG_FATAL("OmnidirectionalShadowMapShader文件读取出错");
			}
			shaderInfo.mVertexShader = vsBlob;
			shaderInfo.mGeometryShader = gsBlob;
			shaderInfo.mPixelShader = psBlob;
			mOmnidirectionalShadowMapShader = std::make_unique<Shader>(shaderInfo, mRHI);
		}

		{
			ShaderInfo shaderInfo;
			std::shared_ptr<Blob> vsBlob = EngineFileSystem::GetInstance()->ReadFile("Shaders/SingleShadowMapVS.cso");
			std::shared_ptr<Blob> psBlob = EngineFileSystem::GetInstance()->ReadFile("Shaders/SingleShadowMapPS.cso");
			if (vsBlob == nullptr || psBlob == nullptr)
			{
				LOG_FATAL("SingleShadowMapShader文件读取出错");
			}
			shaderInfo.mVertexShader = vsBlob;
			shaderInfo.mPixelShader = psBlob;
			mSingleShadowMapShader = std::make_unique<Shader>(shaderInfo, mRHI);
		}

		{
			ShaderInfo shaderInfo;
			std::shared_ptr<Blob> vsBlob = EngineFileSystem::GetInstance()->ReadFile("Shaders/IBLEnvironmentVS.cso");
			std::shared_ptr<Blob> psBlob = EngineFileSystem::GetInstance()->ReadFile("Shaders/IBLEnvironmentPS.cso");
			if (vsBlob == nullptr || psBlob == nullptr)
			{
				LOG_FATAL("IBLEnvironmentShader文件读取出错");
			}
			shaderInfo.mVertexShader = vsBlob;
			shaderInfo.mPixelShader = psBlob;
			mIBLEnvironmentShader = std::make_unique<Shader>(shaderInfo, mRHI);
		}

		{
			ShaderInfo shaderInfo;
			std::shared_ptr<Blob> vsBlob = EngineFileSystem::GetInstance()->ReadFile("Shaders/IBLIrradianceVS.cso");
			std::shared_ptr<Blob> psBlob = EngineFileSystem::GetInstance()->ReadFile("Shaders/IBLIrradiancePS.cso");
			if (vsBlob == nullptr || psBlob == nullptr)
			{
				LOG_FATAL("IBLIrradianceShader文件读取出错");
			}
			shaderInfo.mVertexShader = vsBlob;
			shaderInfo.mPixelShader = psBlob;
			mIBLIrradianceShader = std::make_unique<Shader>(shaderInfo, mRHI);
		}

		{
			ShaderInfo shaderInfo;
			std::shared_ptr<Blob> vsBlob = EngineFileSystem::GetInstance()->ReadFile("Shaders/IBLPrefilterVS.cso");
			std::shared_ptr<Blob> psBlob = EngineFileSystem::GetInstance()->ReadFile("Shaders/IBLPrefilterPS.cso");
			if (vsBlob == nullptr || psBlob == nullptr)
			{
				LOG_FATAL("IBLPrefilterShader文件读取出错");
			}
			shaderInfo.mVertexShader = vsBlob;
			shaderInfo.mPixelShader = psBlob;
			mIBLPrefilterShader = std::make_unique<Shader>(shaderInfo, mRHI);
		}

		{
			ShaderInfo shaderInfo;
			std::shared_ptr<Blob> vsBlob = EngineFileSystem::GetInstance()->ReadFile("Shaders/BasePassSkyVS.cso");
			std::shared_ptr<Blob> psBlob = EngineFileSystem::GetInstance()->ReadFile("Shaders/BasePassSkyPS.cso");
			if (vsBlob == nullptr || psBlob == nullptr)
			{
				LOG_FATAL("BasePassSkyShader文件读取出错");
			}
			shaderInfo.mVertexShader = vsBlob;
			shaderInfo.mPixelShader = psBlob;
			mSkyBoxPassShader = std::make_unique<Shader>(shaderInfo, mRHI);
		}

		{
			ShaderInfo shaderInfo;
			std::shared_ptr<Blob> vsBlob = EngineFileSystem::GetInstance()->ReadFile("Shaders/SSAOVS.cso");
			std::shared_ptr<Blob> psBlob = EngineFileSystem::GetInstance()->ReadFile("Shaders/SSAOPS.cso");
			if (vsBlob == nullptr || psBlob == nullptr)
			{
				LOG_FATAL("SSAOShader文件读取出错");
			}
			shaderInfo.mVertexShader = vsBlob;
			shaderInfo.mPixelShader = psBlob;
			mSSAOShader = std::make_unique<Shader>(shaderInfo, mRHI);
		}

		{
			ShaderInfo shaderInfo;
			std::shared_ptr<Blob> vsBlob = EngineFileSystem::GetInstance()->ReadFile("Shaders/DeferredLightingVS.cso");
			std::shared_ptr<Blob> psBlob = EngineFileSystem::GetInstance()->ReadFile("Shaders/DeferredLightingPS.cso");
			if (vsBlob == nullptr || psBlob == nullptr)
			{
				LOG_FATAL("DeferredLightingShader文件读取出错");
			}
			shaderInfo.mVertexShader = vsBlob;
			shaderInfo.mPixelShader = psBlob;
			mDeferredLightingShader = std::make_unique<Shader>(shaderInfo, mRHI);
		}

		{
			ShaderInfo shaderInfo;
			std::shared_ptr<Blob> vsBlob = EngineFileSystem::GetInstance()->ReadFile("Shaders/TAAVS.cso");
			std::shared_ptr<Blob> psBlob = EngineFileSystem::GetInstance()->ReadFile("Shaders/TAAPS.cso");
			if (vsBlob == nullptr || psBlob == nullptr)
			{
				LOG_FATAL("TAAShader文件读取出错");
			}
			shaderInfo.mVertexShader = vsBlob;
			shaderInfo.mPixelShader = psBlob;
			mTAAShader = std::make_unique<Shader>(shaderInfo, mRHI);
		}

		{
			ShaderInfo shaderInfo;
			std::shared_ptr<Blob> vsBlob = EngineFileSystem::GetInstance()->ReadFile("Shaders/PostProcessVS.cso");
			std::shared_ptr<Blob> psBlob = EngineFileSystem::GetInstance()->ReadFile("Shaders/PostProcessPS.cso");
			if (vsBlob == nullptr || psBlob == nullptr)
			{
				LOG_FATAL("PostProcessShader文件读取出错");
			}
			shaderInfo.mVertexShader = vsBlob;
			shaderInfo.mPixelShader = psBlob;
			mPostProcessShader = std::make_unique<Shader>(shaderInfo, mRHI);
		}

		{
			ShaderInfo shaderInfo;
			std::shared_ptr<Blob> csBlob = EngineFileSystem::GetInstance()->ReadFile("Shaders/BlurHorzCS.cso");
			if (csBlob == nullptr)
			{
				LOG_FATAL("BlurHorzCSShader文件读取出错");
			}
			shaderInfo.mComputeShader = csBlob;
			mHorzBlurShader = std::make_unique<Shader>(shaderInfo, mRHI);
		}

		{
			ShaderInfo shaderInfo;
			std::shared_ptr<Blob> csBlob = EngineFileSystem::GetInstance()->ReadFile("Shaders/BlurVertCS.cso");
			if (csBlob == nullptr)
			{
				LOG_FATAL("BlurVertCSShader文件读取出错");
			}
			shaderInfo.mComputeShader = csBlob;
			mVertBlurShader = std::make_unique<Shader>(shaderInfo, mRHI);
		}
	}

	void D3D12DeferredRenderPipeline::CreateGlobalPSO()
	{
		// GraphicsPSO
		{
			mZPrePassPSODescriptor.mInputLayoutName = "BaseInputLayout";
			mZPrePassPSODescriptor.mShader = mZPrePassShader.get();
			mZPrePassPSODescriptor.mPrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			mZPrePassPSODescriptor.mRTVFormats[0] = DXGI_FORMAT_UNKNOWN;
			mZPrePassPSODescriptor.mNumRenderTargets = 0;
			mZPrePassPSODescriptor.mRasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			mZPrePassPSODescriptor.mDepthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			mZPrePassPSODescriptor.mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
			mRenderResource->TryCreatePSO(mZPrePassPSODescriptor);
		}

		{
			mOmnidirectionalShadowMapPSODescriptor.mInputLayoutName = "BaseInputLayout";
			mOmnidirectionalShadowMapPSODescriptor.mShader = mOmnidirectionalShadowMapShader.get();
			mOmnidirectionalShadowMapPSODescriptor.mPrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			mOmnidirectionalShadowMapPSODescriptor.mRTVFormats[0] = DXGI_FORMAT_UNKNOWN;
			mOmnidirectionalShadowMapPSODescriptor.mNumRenderTargets = 0;
			mOmnidirectionalShadowMapPSODescriptor.mRasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			mOmnidirectionalShadowMapPSODescriptor.mDepthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			mOmnidirectionalShadowMapPSODescriptor.mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
			mOmnidirectionalShadowMapPSODescriptor.mRasterizerDesc.DepthBias = 100000;
			mOmnidirectionalShadowMapPSODescriptor.mRasterizerDesc.DepthBiasClamp = 0.0f;
			mOmnidirectionalShadowMapPSODescriptor.mRasterizerDesc.SlopeScaledDepthBias = 1.0f;
			mRenderResource->TryCreatePSO(mOmnidirectionalShadowMapPSODescriptor);
		}

		{
			mSingleShadowMapPSODescriptor.mInputLayoutName = "BaseInputLayout";
			mSingleShadowMapPSODescriptor.mShader = mSingleShadowMapShader.get();
			mSingleShadowMapPSODescriptor.mPrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			mSingleShadowMapPSODescriptor.mRTVFormats[0] = DXGI_FORMAT_UNKNOWN;
			mSingleShadowMapPSODescriptor.mNumRenderTargets = 0;
			mSingleShadowMapPSODescriptor.mRasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			mSingleShadowMapPSODescriptor.mDepthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			mSingleShadowMapPSODescriptor.mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
			mSingleShadowMapPSODescriptor.mRasterizerDesc.DepthBias = 100000;
			mSingleShadowMapPSODescriptor.mRasterizerDesc.DepthBiasClamp = 0.0f;
			mSingleShadowMapPSODescriptor.mRasterizerDesc.SlopeScaledDepthBias = 1.0f;
			mRenderResource->TryCreatePSO(mSingleShadowMapPSODescriptor);
		}

		{
			mIBLEnvironmentPSODescriptor.mInputLayoutName = "BaseInputLayout";
			mIBLEnvironmentPSODescriptor.mShader = mIBLEnvironmentShader.get();
			mIBLEnvironmentPSODescriptor.mPrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			mIBLEnvironmentPSODescriptor.mRTVFormats[0] = mIBLEnvironmentMap->GetRTCube()->GetFormat();
			mIBLEnvironmentPSODescriptor.mNumRenderTargets = 1;
			// The camera is inside the cube, so just turn off culling.
			mIBLEnvironmentPSODescriptor.mRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
			mIBLEnvironmentPSODescriptor.mDepthStencilDesc.DepthEnable = false;
			mIBLEnvironmentPSODescriptor.mDepthStencilFormat = DXGI_FORMAT_UNKNOWN;

			mRenderResource->TryCreatePSO(mIBLEnvironmentPSODescriptor);
		}

		{
			mIBLIrradiancePSODescriptor.mInputLayoutName = "BaseInputLayout";
			mIBLIrradiancePSODescriptor.mShader = mIBLIrradianceShader.get();
			mIBLIrradiancePSODescriptor.mPrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			mIBLIrradiancePSODescriptor.mRTVFormats[0] = mIBLIrradianceMap->GetRTCube()->GetFormat();
			mIBLIrradiancePSODescriptor.mNumRenderTargets = 1;
			// The camera is inside the cube, so just turn off culling.
			mIBLIrradiancePSODescriptor.mRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
			mIBLIrradiancePSODescriptor.mDepthStencilDesc.DepthEnable = false;
			mIBLIrradiancePSODescriptor.mDepthStencilFormat = DXGI_FORMAT_UNKNOWN;

			mRenderResource->TryCreatePSO(mIBLIrradiancePSODescriptor);
		}

		{
			mIBLPrefilterPSODescriptor.mInputLayoutName = "BaseInputLayout";
			mIBLPrefilterPSODescriptor.mShader = mIBLPrefilterShader.get();
			mIBLPrefilterPSODescriptor.mPrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			mIBLPrefilterPSODescriptor.mRTVFormats[0] = mIBLPrefilterMap[0]->GetRTCube()->GetFormat();
			mIBLPrefilterPSODescriptor.mNumRenderTargets = 1;
			// The camera is inside the cube, so just turn off culling.
			mIBLPrefilterPSODescriptor.mRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
			mIBLPrefilterPSODescriptor.mDepthStencilDesc.DepthEnable = false;
			mIBLPrefilterPSODescriptor.mDepthStencilFormat = DXGI_FORMAT_UNKNOWN;

			mRenderResource->TryCreatePSO(mIBLPrefilterPSODescriptor);
		}

		{
			mSkyBoxPassPSODescriptor.mInputLayoutName = "BaseInputLayout";
			mSkyBoxPassPSODescriptor.mShader = mSkyBoxPassShader.get();
			mSkyBoxPassPSODescriptor.mPrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			mSkyBoxPassPSODescriptor.mRTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;	// Position
			mSkyBoxPassPSODescriptor.mRTVFormats[1] = DXGI_FORMAT_R8G8B8A8_SNORM;		// Normal
			mSkyBoxPassPSODescriptor.mRTVFormats[2] = DXGI_FORMAT_R32G32B32A32_FLOAT;	// BaseColor
			mSkyBoxPassPSODescriptor.mRTVFormats[3] = DXGI_FORMAT_R8G8B8A8_UNORM;		// MetallicRoughness
			mSkyBoxPassPSODescriptor.mRTVFormats[4] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Emissive
			mSkyBoxPassPSODescriptor.mRTVFormats[5] = DXGI_FORMAT_R16G16_FLOAT;			// Velocity
			mSkyBoxPassPSODescriptor.mNumRenderTargets = 6;
			mSkyBoxPassPSODescriptor.mRasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			mSkyBoxPassPSODescriptor.mRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
			mSkyBoxPassPSODescriptor.mDepthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			mSkyBoxPassPSODescriptor.mDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
			//mSkyBoxPassPSODescriptor.mDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
			mSkyBoxPassPSODescriptor.mDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
			mSkyBoxPassPSODescriptor.mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
			mRenderResource->TryCreatePSO(mSkyBoxPassPSODescriptor);
		}

		{
			mSSAOPSODescriptor.mInputLayoutName = "BaseInputLayout";
			mSSAOPSODescriptor.m4xMsaaState = false;
			mSSAOPSODescriptor.mDepthStencilDesc.DepthEnable = false;
			mSSAOPSODescriptor.mPrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			mSSAOPSODescriptor.mRTVFormats[0] = DXGI_FORMAT_R16_UNORM;
			mSSAOPSODescriptor.mNumRenderTargets = 1;
			mSSAOPSODescriptor.mRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
			mSSAOPSODescriptor.mRasterizerDesc.DepthClipEnable = false;
			mSSAOPSODescriptor.mShader = mSSAOShader.get();

			mRenderResource->TryCreatePSO(mSSAOPSODescriptor);
		}

		{
			D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			blendDesc.AlphaToCoverageEnable = false;
			blendDesc.IndependentBlendEnable = false;
			blendDesc.RenderTarget[0].BlendEnable = true;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;


			D3D12_DEPTH_STENCIL_DESC depthDesc;
			depthDesc.DepthEnable = false;
			depthDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
			depthDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
			depthDesc.StencilEnable = true;
			depthDesc.StencilReadMask = 0xff;
			depthDesc.StencilWriteMask = 0x0;
			depthDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
			depthDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
			depthDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
			depthDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
			// We are not rendering backfacing polygons, so these settings do not matter. 
			depthDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
			depthDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
			depthDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
			depthDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
			
			mDeferredLightingPSODescriptor.mInputLayoutName = "BaseInputLayout";
			mDeferredLightingPSODescriptor.m4xMsaaState = false;
			mDeferredLightingPSODescriptor.mDepthStencilDesc.DepthEnable = false;
			mDeferredLightingPSODescriptor.mPrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			mDeferredLightingPSODescriptor.mRTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
			mDeferredLightingPSODescriptor.mNumRenderTargets = 1;
			mDeferredLightingPSODescriptor.mRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
			mDeferredLightingPSODescriptor.mRasterizerDesc.DepthClipEnable = false;
			mDeferredLightingPSODescriptor.mBlendDesc = blendDesc;
			mDeferredLightingPSODescriptor.mDepthStencilDesc = depthDesc;
			mDeferredLightingPSODescriptor.mShader = mDeferredLightingShader.get();

			mRenderResource->TryCreatePSO(mDeferredLightingPSODescriptor);
		}

		{
			mPostProcessPSODescriptor.mInputLayoutName = "BaseInputLayout";
			mPostProcessPSODescriptor.m4xMsaaState = false;
			mPostProcessPSODescriptor.mDepthStencilDesc.DepthEnable = false;
			mPostProcessPSODescriptor.mDepthStencilFormat = DXGI_FORMAT_UNKNOWN;
			mPostProcessPSODescriptor.mPrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			mPostProcessPSODescriptor.mRTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			mPostProcessPSODescriptor.mNumRenderTargets = 1;
			mPostProcessPSODescriptor.mRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
			mPostProcessPSODescriptor.mShader = mPostProcessShader.get();
			
			mRenderResource->TryCreatePSO(mPostProcessPSODescriptor);
		}

		{
			mTAAPSODescriptor.mInputLayoutName = "BaseInputLayout";
			mTAAPSODescriptor.m4xMsaaState = false;
			mTAAPSODescriptor.mDepthStencilDesc.DepthEnable = false;
			mTAAPSODescriptor.mDepthStencilFormat = DXGI_FORMAT_UNKNOWN;
			mTAAPSODescriptor.mPrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			mTAAPSODescriptor.mRTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
			mTAAPSODescriptor.mNumRenderTargets = 1;
			mTAAPSODescriptor.mRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
			mTAAPSODescriptor.mShader = mTAAShader.get();

			mRenderResource->TryCreatePSO(mTAAPSODescriptor);
		}

		// ComputePSO

		// Blur
		{
			mVertBlurPSODescriptor.mShader = mVertBlurShader.get();
			mVertBlurPSODescriptor.mFlags = D3D12_PIPELINE_STATE_FLAG_NONE;
			mRenderResource->TryCreatePSO(mVertBlurPSODescriptor);

			mHorzBlurPSODescriptor.mShader = mHorzBlurShader.get();
			mHorzBlurPSODescriptor.mFlags = D3D12_PIPELINE_STATE_FLAG_NONE;
			mRenderResource->TryCreatePSO(mHorzBlurPSODescriptor);
		}
	}

	void D3D12DeferredRenderPipeline::SetDescriptorHeaps()
	{
		auto cacheCbvSrvUavDescriptorHeap = mRHI->GetDevice()->GetCommandContext()->GetDescriptorCache()->GetCacheCbvSrvUavDescriptorHeap();
		ID3D12DescriptorHeap* descriptorHeaps[] = { cacheCbvSrvUavDescriptorHeap.Get() };

		mRHI->GetDevice()->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE D3D12DeferredRenderPipeline::CurrentBackBufferView() const
	{
		if (mSceneRenderTarget == nullptr)
		{
			return mRHI->GetViewport()->GetCurrentBackBufferView()->GetDescriptorHandle();
		}
		else
		{
			return mSceneRenderTarget->GetRTV()->GetDescriptorHandle();
		}
	}

	D3D12Resource* D3D12DeferredRenderPipeline::CurrentBackBuffer() const
	{
		if (mSceneRenderTarget == nullptr)
		{
			return mRHI->GetViewport()->GetCurrentBackBuffer();
		}
		else
		{
			return mSceneRenderTarget->GetResource();
		}
	}

	float* D3D12DeferredRenderPipeline::CurrentBackBufferClearColor() const
	{
		if (mSceneRenderTarget == nullptr)
		{
			return mRHI->GetViewport()->GetCurrentBackBufferClearColor();
		}
		else
		{
			return mSceneRenderTarget->GetClearColorPtr();
		}
	}

	D3D12_CPU_DESCRIPTOR_HANDLE D3D12DeferredRenderPipeline::DepthStencilView() const
	{
		return mDepthTestBuffer->GetDSV()->GetDescriptorHandle();
		//return mRHI->GetViewport()->GetDepthStencilView()->GetDescriptorHandle();
	}

	void D3D12DeferredRenderPipeline::ShadowsPass()
	{
		auto& shadowParameters = mRenderResource->GetShadowParameters();

		mCurrentShadowMap2DIndex = 0;

		mCurrentShadowMapCubeIndex = 0;

		for (int i = 0; i < shadowParameters.size(); ++i)
		{
			ShadowParameter& shadowParameter = shadowParameters[i];

			if (shadowParameter.Type == ShadowParameter::emDirectionalShadowMap)
			{
				GenerateDirectionalShadowMap(shadowParameter);
				++mCurrentShadowMap2DIndex;
			}
			else if (shadowParameter.Type == ShadowParameter::emPointShadowMap)
			{
				GenerateOmnidirectionalShadowMap(shadowParameter);
				++mCurrentShadowMapCubeIndex;
			}
			else
			{
				ASSERT(0);
			}
		}
	}

	void D3D12DeferredRenderPipeline::DepthPrePass()
	{
		auto commandList = mRHI->GetDevice()->GetCommandList();

		commandList->RSSetViewports(1, &mScreenViewport);
		commandList->RSSetScissorRects(1, &mScissorRect);

		commandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		commandList->OMSetRenderTargets(0, nullptr, true, &DepthStencilView());

		auto cbPassRef = mRenderResource->GetCbPassRef();

		const auto& batchs = mRenderResource->GetBasePassBatchs();

		ID3D12PipelineState* pipeline_state = mRenderResource->GetPSO(mZPrePassPSODescriptor);
		commandList->SetPipelineState(pipeline_state);
		commandList->SetGraphicsRootSignature(mZPrePassShader->mRootSignature.Get());
			
		for (const auto& [materialResource, subMeshResource, modleResource, InstanceCount] : batchs)
		{
			mZPrePassShader->SetParameter("gInstanceData", modleResource.gInstanceDataD3D12StructuredBufferRef->GetSRV());
			mZPrePassShader->SetParameter("gBoneTransforms", modleResource.gBoneTransformsD3D12StructuredBufferRef->GetSRV());
			mZPrePassShader->SetParameter("cbPass", cbPassRef);
			mZPrePassShader->BindParameters();

			mRHI->SetVertexBuffer(subMeshResource.VertexBufferRef, 0, subMeshResource.VertexByteStride, subMeshResource.VertexBufferByteSize);
			mRHI->SetIndexBuffer(subMeshResource.IndexBufferRef, 0, subMeshResource.IndexFormat, subMeshResource.IndexBufferByteSize);

			commandList->IASetPrimitiveTopology(subMeshResource.PrimitiveType);

			commandList->DrawIndexedInstanced(subMeshResource.IndexCount, InstanceCount, subMeshResource.StartIndexLocation, subMeshResource.BaseVertexLocation, 0);
			++RenderSystem::GetInstance()->mDrawCall;
		}
	}

	void D3D12DeferredRenderPipeline::BasePass()
	{
		auto commandList = mRHI->GetDevice()->GetCommandList();

		mRHI->TransitionResource(mPositionGBuffer->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		mRHI->TransitionResource(mNormalGBuffer->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		mRHI->TransitionResource(mBaseColorGBuffer->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		mRHI->TransitionResource(mMetallicRoughnessGBuffer->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		mRHI->TransitionResource(mEmissiveGBuffer->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		mRHI->TransitionResource(mVelocityBuffer->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);

		commandList->ClearRenderTargetView(mPositionGBuffer->GetRTV()->GetDescriptorHandle(), mPositionGBuffer->GetClearColorPtr(), 0, nullptr);
		commandList->ClearRenderTargetView(mNormalGBuffer->GetRTV()->GetDescriptorHandle(), mNormalGBuffer->GetClearColorPtr(), 0, nullptr);
		commandList->ClearRenderTargetView(mBaseColorGBuffer->GetRTV()->GetDescriptorHandle(), mBaseColorGBuffer->GetClearColorPtr(), 0, nullptr);
		commandList->ClearRenderTargetView(mMetallicRoughnessGBuffer->GetRTV()->GetDescriptorHandle(), mMetallicRoughnessGBuffer->GetClearColorPtr(), 0, nullptr);
		commandList->ClearRenderTargetView(mEmissiveGBuffer->GetRTV()->GetDescriptorHandle(), mEmissiveGBuffer->GetClearColorPtr(), 0, nullptr);
		commandList->ClearRenderTargetView(mVelocityBuffer->GetRTV()->GetDescriptorHandle(), mVelocityBuffer->GetClearColorPtr(), 0, nullptr);

		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvDescriptors;
		rtvDescriptors.push_back(mPositionGBuffer->GetRTV()->GetDescriptorHandle());
		rtvDescriptors.push_back(mNormalGBuffer->GetRTV()->GetDescriptorHandle());
		rtvDescriptors.push_back(mBaseColorGBuffer->GetRTV()->GetDescriptorHandle());
		rtvDescriptors.push_back(mMetallicRoughnessGBuffer->GetRTV()->GetDescriptorHandle());
		rtvDescriptors.push_back(mEmissiveGBuffer->GetRTV()->GetDescriptorHandle());
		rtvDescriptors.push_back(mVelocityBuffer->GetRTV()->GetDescriptorHandle());

		auto descriptorCache = mRHI->GetDevice()->GetCommandContext()->GetDescriptorCache();
		CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle = descriptorCache->AppendRtvDescriptors(rtvDescriptors);

		commandList->OMSetRenderTargets(6, &cpuHandle, true, &DepthStencilView());

		const auto& batchs = mRenderResource->GetBasePassBatchs();

		for (const auto & [materialResource, subMeshResource, modleResource, InstanceCount] : batchs)
		{
			const GraphicsPSODescriptor& Descriptor = materialResource.Descriptor;
			ID3D12PipelineState* pipeline_state = mRenderResource->GetPSO(Descriptor);
			ASSERT(pipeline_state != nullptr);
			commandList->SetPipelineState(pipeline_state);

			commandList->SetGraphicsRootSignature(Descriptor.mShader->mRootSignature.Get());

			materialResource.BindShaderBindParameters(modleResource);

			mRHI->SetVertexBuffer(subMeshResource.VertexBufferRef, 0, subMeshResource.VertexByteStride, subMeshResource.VertexBufferByteSize);
			mRHI->SetIndexBuffer(subMeshResource.IndexBufferRef, 0, subMeshResource.IndexFormat, subMeshResource.IndexBufferByteSize);

			commandList->IASetPrimitiveTopology(subMeshResource.PrimitiveType);

			commandList->DrawIndexedInstanced(subMeshResource.IndexCount, InstanceCount, subMeshResource.StartIndexLocation, subMeshResource.BaseVertexLocation, 0);
			++RenderSystem::GetInstance()->mDrawCall;
		}
		
		{
			Mesh* load_default_mesh_resource = AssetManager::GetInstance()->LoadDefaultMeshResource(DefaultMesh_Sphere);
			ASSERT(load_default_mesh_resource != nullptr);
			SubMesh& refSubMesh = load_default_mesh_resource->Meshes[0];
			SubMeshResource subMesh;
			subMesh.VertexByteStride = sizeof(Vertex);
			subMesh.VertexBufferByteSize = subMesh.VertexByteStride * refSubMesh.Vertices.size();
			subMesh.IndexFormat = DXGI_FORMAT_R32_UINT;
			subMesh.IndexBufferByteSize = sizeof(unsigned) * refSubMesh.Indices.size();
			subMesh.PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

			subMesh.VertexBufferRef = mRenderResource->GetMeshVertexBufferRef(AssetManager::GetInstance()->GetDefaultAssetGuid(DefaultMesh_Sphere));
			subMesh.IndexBufferRef = mRenderResource->GetMeshIndexBufferRef(AssetManager::GetInstance()->GetDefaultAssetGuid(DefaultMesh_Sphere));

			subMesh.IndexCount = refSubMesh.Indices.size();
			subMesh.StartIndexLocation = 0;
			subMesh.BaseVertexLocation = 0;

			ID3D12PipelineState* pipeline_state = mRenderResource->GetPSO(mSkyBoxPassPSODescriptor);
			ASSERT(pipeline_state != nullptr);
			commandList->SetPipelineState(pipeline_state);

			commandList->SetGraphicsRootSignature(mSkyBoxPassPSODescriptor.mShader->mRootSignature.Get());

			mSkyBoxPassPSODescriptor.mShader->SetParameter("cbPass", mRenderResource->GetCbPassRef());
			mSkyBoxPassPSODescriptor.mShader->SetParameter("SkyCubeTexture", mIBLEnvironmentMap->GetRTCube()->GetSRV());
			//mSkyBoxPassPSODescriptor.mShader->SetParameter("SkyCubeTexture", mIBLEnviromentTexture->GetSRV());
			mSkyBoxPassPSODescriptor.mShader->BindParameters();
			
			mRHI->SetVertexBuffer(subMesh.VertexBufferRef, 0, subMesh.VertexByteStride, subMesh.VertexBufferByteSize);
			mRHI->SetIndexBuffer(subMesh.IndexBufferRef, 0, subMesh.IndexFormat, subMesh.IndexBufferByteSize);

			commandList->IASetPrimitiveTopology(subMesh.PrimitiveType);

			commandList->DrawIndexedInstanced(subMesh.IndexCount, 1, subMesh.StartIndexLocation, subMesh.BaseVertexLocation, 0);
			++RenderSystem::GetInstance()->mDrawCall;
		}
		
		mRHI->TransitionResource(mPositionGBuffer->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
		mRHI->TransitionResource(mNormalGBuffer->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
		mRHI->TransitionResource(mBaseColorGBuffer->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
		mRHI->TransitionResource(mMetallicRoughnessGBuffer->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
		mRHI->TransitionResource(mEmissiveGBuffer->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
		mRHI->TransitionResource(mVelocityBuffer->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
	}

	void D3D12DeferredRenderPipeline::SSAOPass()
	{
		auto commandList = mRHI->GetDevice()->GetCommandList();

		mRHI->TransitionResource(mSSAOTexture->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);

		commandList->ClearRenderTargetView(mSSAOTexture->GetRTV()->GetDescriptorHandle(), mSSAOTexture->GetClearColorPtr(), 0, nullptr);

		commandList->OMSetRenderTargets(1, &mSSAOTexture->GetRTV()->GetDescriptorHandle(), true, nullptr);

		ID3D12PipelineState* pipeline_state = mRenderResource->GetPSO(mSSAOPSODescriptor);
		ASSERT(pipeline_state != nullptr);
		commandList->SetPipelineState(pipeline_state);

		commandList->SetGraphicsRootSignature(mSSAOPSODescriptor.mShader->mRootSignature.Get());

		mSSAOPSODescriptor.mShader->SetParameter("cbPass", mRenderResource->GetCbPassRef());
		mSSAOPSODescriptor.mShader->SetParameter("cbSSAO", mRenderResource->GetSSAOCBRef());
		mSSAOPSODescriptor.mShader->SetParameter("NormalGBuffer", mNormalGBuffer->GetSRV());
		mSSAOPSODescriptor.mShader->SetParameter("DepthGBuffer", mDepthTestBuffer->GetSRV());

		mSSAOPSODescriptor.mShader->BindParameters();

		DrawCallScreen(commandList);
		
		// Blur
		// Horz
		pipeline_state = mRenderResource->GetPSO(mHorzBlurPSODescriptor);

		commandList->SetPipelineState(pipeline_state);

		commandList->SetComputeRootSignature(mHorzBlurPSODescriptor.mShader->mRootSignature.Get());
		
		mRHI->TransitionResource(mSSAOTexture->GetTexture()->GetResource(), D3D12_RESOURCE_STATE_COPY_SOURCE);

		mRHI->TransitionResource(mBlur1SSAOTexture->GetResource(), D3D12_RESOURCE_STATE_COPY_DEST);

		mRHI->CopyResource(mBlur1SSAOTexture->GetResource(), mSSAOTexture->GetTexture()->GetResource());

		mRHI->TransitionResource(mBlur1SSAOTexture->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);

		mHorzBlurPSODescriptor.mShader->SetParameter("cbBlurSetting", mSSAOBlurCBRef);
		mHorzBlurPSODescriptor.mShader->SetParameter("InputTexture", mBlur1SSAOTexture->GetSRV());
		mHorzBlurPSODescriptor.mShader->SetParameter("OutputTexture", mBlur2SSAOTexture->GetUAV());
		mHorzBlurPSODescriptor.mShader->BindParameters();

		commandList->Dispatch(ceilf(mTargetWight / 256.0f), mTargetHeight, 1);

		// Vert
		pipeline_state = mRenderResource->GetPSO(mVertBlurPSODescriptor);

		commandList->SetPipelineState(pipeline_state);

		commandList->SetComputeRootSignature(mVertBlurPSODescriptor.mShader->mRootSignature.Get());

		mRHI->TransitionResource(mSSAOTexture->GetTexture()->GetResource(), D3D12_RESOURCE_STATE_COPY_SOURCE);

		mRHI->TransitionResource(mBlur1SSAOTexture->GetResource(), D3D12_RESOURCE_STATE_COPY_DEST);

		mRHI->CopyResource(mBlur1SSAOTexture->GetResource(), mSSAOTexture->GetTexture()->GetResource());

		mRHI->TransitionResource(mBlur1SSAOTexture->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);

		mVertBlurPSODescriptor.mShader->SetParameter("cbBlurSetting", mSSAOBlurCBRef);
		mVertBlurPSODescriptor.mShader->SetParameter("InputTexture", mBlur2SSAOTexture->GetSRV());
		mVertBlurPSODescriptor.mShader->SetParameter("OutputTexture", mBlur1SSAOTexture->GetUAV());
		mVertBlurPSODescriptor.mShader->BindParameters();

		commandList->Dispatch(mTargetWight, ceilf(mTargetHeight / 256.0f), 1);

		mRHI->TransitionResource(mBlur1SSAOTexture->GetResource(), D3D12_RESOURCE_STATE_COPY_SOURCE);

		mRHI->TransitionResource(mSSAOTexture->GetResource(), D3D12_RESOURCE_STATE_COPY_DEST);

		mRHI->CopyResource(mSSAOTexture->GetResource(), mBlur1SSAOTexture->GetResource());
		
		mRHI->TransitionResource(mSSAOTexture->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
	}

	void D3D12DeferredRenderPipeline::DeferredLightingPass()
	{
		auto commandList = mRHI->GetDevice()->GetCommandList();

		mRHI->TransitionResource(mColorTexture->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);

		commandList->ClearRenderTargetView(mColorTexture->GetRTV()->GetDescriptorHandle(), mColorTexture->GetClearColorPtr(), 0, nullptr);

		commandList->OMSetRenderTargets(1, &mColorTexture->GetRTV()->GetDescriptorHandle(), true, &DepthStencilView());

		commandList->SetPipelineState(mRenderResource->GetPSO(mDeferredLightingPSODescriptor));

		Shader* shader = mDeferredLightingPSODescriptor.mShader;

		commandList->SetGraphicsRootSignature(shader->mRootSignature.Get());

		shader->SetParameter("PositionGBuffer", mPositionGBuffer->GetSRV());
		shader->SetParameter("NormalGBuffer", mNormalGBuffer->GetSRV());
		shader->SetParameter("ColorGBuffer", mBaseColorGBuffer->GetSRV());
		shader->SetParameter("MetallicRoughnessGBuffer", mMetallicRoughnessGBuffer->GetSRV());
		shader->SetParameter("EmissiveGBuffer", mEmissiveGBuffer->GetSRV());

		shader->SetParameter("EnvBRDFLUT", mBRDFLUTTexture->GetSRV());
		shader->SetParameter("IBLIrradianceMap", mIBLIrradianceMap->GetRTCube()->GetSRV());

		std::vector<D3D12ShaderResourceView*> shadowMap2DSRVs;
		for (int i = 0; i < mCurrentShadowMap2DIndex; ++i)
		{
			shadowMap2DSRVs.push_back(mShadowMap2D[i]->GetTarget()->GetSRV());
		}
		for (int i = mCurrentShadowMap2DIndex; i < MAX_2DSHADOWMAP; ++i)
		{
			shadowMap2DSRVs.push_back(mRenderResource->mNullTextureResource->GetSRV());
		}
		shader->SetParameter("ShadowMaps2D", shadowMap2DSRVs);

		std::vector<D3D12ShaderResourceView*> shadowMapCubeSRVs;
		for (int i = 0; i < mCurrentShadowMapCubeIndex; ++i)
		{
			shadowMapCubeSRVs.push_back(mShadowMapCube[i]->GetTarget()->GetSRV());
		}
		for (int i = mCurrentShadowMapCubeIndex; i < MAX_CubeSHADOWMAP; ++i)
		{
			shadowMapCubeSRVs.push_back(mRenderResource->mNullTextureResource->GetSRV());
		}
		shader->SetParameter("ShadowMapsCube", shadowMapCubeSRVs);

		std::vector<D3D12ShaderResourceView*> IBLPrefilterEnvMapSRVs;
		for (UINT i = 0; i < mIBLPrefilterMaxMipLevel; i++)
		{
			IBLPrefilterEnvMapSRVs.push_back(mIBLPrefilterMap[i]->GetRTCube()->GetSRV());
		}
		shader->SetParameter("IBLPrefilterEnvMaps", IBLPrefilterEnvMapSRVs);

		if (RenderSystem::GetInstance()->mEnableSSAO)
		{
			shader->SetParameter("SSAOBuffer", mSSAOTexture->GetSRV());
		}
		else
		{
			shader->SetParameter("SSAOBuffer", mRenderResource->mNullTextureResource->GetSRV());
		}

		shader->SetParameter("cbPass", mRenderResource->GetCbPassRef());
		if (mRenderResource->mDirectionalLightStructuredBufferRef != nullptr)
		{
			shader->SetParameter("DirectionalLightList", mRenderResource->mDirectionalLightStructuredBufferRef->GetSRV());
		}
		else
		{
			shader->SetParameter("DirectionalLightList", mRenderResource->mNullSRV->GetSRV());
		}
		if (mRenderResource->mPointLightStructuredBufferRef)
		{
			shader->SetParameter("PointLightList", mRenderResource->mPointLightStructuredBufferRef->GetSRV());
		}
		else
		{
			shader->SetParameter("PointLightList", mRenderResource->mNullSRV->GetSRV());
		}
		if (mRenderResource->mSpotLightStructuredBufferRef)
		{
			shader->SetParameter("SpotLightList", mRenderResource->mSpotLightStructuredBufferRef->GetSRV());
		}
		else
		{
			shader->SetParameter("SpotLightList", mRenderResource->mNullSRV->GetSRV());
		}
		shader->SetParameter("cbLightCommon", mRenderResource->mLightCommonDataBuffer);

		shader->BindParameters();

		DrawCallScreen(commandList);

		mRHI->TransitionResource(mColorTexture->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
	}

	void D3D12DeferredRenderPipeline::TAAPass()
	{
		auto commandList = mRHI->GetDevice()->GetCommandList();

		// 将ColorBuffer复制到Cache中
		mRHI->TransitionResource(mColorTexture->GetResource(), D3D12_RESOURCE_STATE_COPY_SOURCE);
		mRHI->TransitionResource(mCacheColorTexture->GetResource(), D3D12_RESOURCE_STATE_COPY_DEST);
		mRHI->CopyResource(mCacheColorTexture->GetResource(), mColorTexture->GetResource());
		mRHI->TransitionResource(mCacheColorTexture->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
		mRHI->TransitionResource(mColorTexture->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);

		// 清除ColorBuffer
		commandList->ClearRenderTargetView(mColorTexture->GetRTV()->GetDescriptorHandle(), mColorTexture->GetClearColorPtr(), 0, nullptr);

		commandList->OMSetRenderTargets(1, &mColorTexture->GetRTV()->GetDescriptorHandle(), true, nullptr);

		ID3D12PipelineState* pipeline_state = mRenderResource->GetPSO(mTAAPSODescriptor);
		ASSERT(pipeline_state != nullptr);
		commandList->SetPipelineState(pipeline_state);

		commandList->SetGraphicsRootSignature(mTAAPSODescriptor.mShader->mRootSignature.Get());

		mTAAPSODescriptor.mShader->SetParameter("cbPass", mRenderResource->GetCbPassRef());
		mTAAPSODescriptor.mShader->SetParameter("ColorBuffer", mCacheColorTexture->GetSRV());
		mTAAPSODescriptor.mShader->SetParameter("PrevColorTexture", mPrevColorTexture->GetSRV());
		mTAAPSODescriptor.mShader->SetParameter("VelocityBuffer", mVelocityBuffer->GetSRV());
		mTAAPSODescriptor.mShader->SetParameter("DepthTexture", mDepthTestBuffer->GetSRV());

		mTAAPSODescriptor.mShader->BindParameters();

		DrawCallScreen(commandList);

		// 缓存当前帧的色彩图 供下一帧使用
		mRHI->TransitionResource(mColorTexture->GetResource(), D3D12_RESOURCE_STATE_COPY_SOURCE);
		mRHI->TransitionResource(mPrevColorTexture->GetResource(), D3D12_RESOURCE_STATE_COPY_DEST);
		mRHI->CopyResource(mPrevColorTexture->GetResource(), mColorTexture->GetResource());
		mRHI->TransitionResource(mPrevColorTexture->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
		mRHI->TransitionResource(mColorTexture->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
	}

	void D3D12DeferredRenderPipeline::PostProcessPass()
	{
		auto commandList = mRHI->GetDevice()->GetCommandList();

		mRHI->TransitionResource(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		
		commandList->ClearRenderTargetView(CurrentBackBufferView(), mSceneRenderTarget->GetClearColorPtr(), 0, nullptr);

		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = mSceneRenderTarget->GetRTV()->GetDescriptorHandle();

		commandList->OMSetRenderTargets(1, &cpuHandle, true, &DepthStencilView());

		commandList->SetPipelineState(mRenderResource->GetPSO(mPostProcessPSODescriptor));

		Shader* shader = mPostProcessPSODescriptor.mShader;

		commandList->SetGraphicsRootSignature(shader->mRootSignature.Get());

		mPostProcessPSODescriptor.mShader->SetParameter("ColorTexture", mColorTexture->GetSRV());
		mPostProcessPSODescriptor.mShader->BindParameters();

		DrawCallScreen(commandList);

		mRHI->TransitionResource(CurrentBackBuffer(), D3D12_RESOURCE_STATE_GENERIC_READ);
	}

	void D3D12DeferredRenderPipeline::ResizeDepthTestBuffer()
	{
		D3D12TextureInfo TextureInfo;
		TextureInfo.Type = IMAGE_TYPE::IMAGE_TYPE_2D;
		TextureInfo.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		TextureInfo.Width = mTargetWight;
		TextureInfo.Height = mTargetHeight;
		TextureInfo.Depth = 1;
		TextureInfo.MipCount = 1;
		TextureInfo.ArraySize = 1;
		TextureInfo.InitState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		TextureInfo.Format = DXGI_FORMAT_R24G8_TYPELESS;  // Create with a typeless format, support DSV and SRV(for SSAO)
		TextureInfo.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		TextureInfo.SRVFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

		mDepthTestBuffer = mRHI->CreateTexture(TextureInfo, TexCreate_DSV | TexCreate_SRV);
		mDepthTestBuffer->GetD3DResource()->SetName(L"DepthTestBuffer");
	}

	void D3D12DeferredRenderPipeline::ResizeGBuffers()
	{
		mPositionGBuffer = std::make_unique<RenderTarget2D>(mRHI, false, mTargetWight, mTargetHeight, DXGI_FORMAT_R32G32B32A32_FLOAT);
		mPositionGBuffer->GetResource()->D3DResource->SetName(L"PositionGBuffer");
		mNormalGBuffer = std::make_unique<RenderTarget2D>(mRHI, false, mTargetWight, mTargetHeight, DXGI_FORMAT_R8G8B8A8_SNORM);
		mNormalGBuffer->GetResource()->D3DResource->SetName(L"NormalGBuffer");
		mBaseColorGBuffer = std::make_unique<RenderTarget2D>(mRHI, false, mTargetWight, mTargetHeight, DXGI_FORMAT_R32G32B32A32_FLOAT);
		mBaseColorGBuffer->GetResource()->D3DResource->SetName(L"BaseColorGBuffer");
		mMetallicRoughnessGBuffer = std::make_unique<RenderTarget2D>(mRHI, false, mTargetWight, mTargetHeight, DXGI_FORMAT_R8G8B8A8_UNORM);
		mMetallicRoughnessGBuffer->GetResource()->D3DResource->SetName(L"MetallicRoughnessGBuffer");
		mEmissiveGBuffer = std::make_unique<RenderTarget2D>(mRHI, false, mTargetWight, mTargetHeight, DXGI_FORMAT_R8G8B8A8_UNORM);
		mEmissiveGBuffer->GetResource()->D3DResource->SetName(L"EmissiveGBuffer");
	}

	void D3D12DeferredRenderPipeline::ResizeRenderTarget()
	{
		mColorTexture = std::make_unique<RenderTarget2D>(mRHI, false, mTargetWight, mTargetHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, Colors::Black);
		mColorTexture->GetResource()->D3DResource->SetName(L"DeferredLighting");

		mSceneRenderTarget = std::make_unique<RenderTarget2D>(mRHI, false, mTargetWight, mTargetHeight, DXGI_FORMAT_R8G8B8A8_UNORM, Colors::Black);
		mSceneRenderTarget->GetResource()->D3DResource->SetName(L"测试 渲染到纹理");

		mImGuiDevice->SetSceneUITexture(mSceneRenderTarget->GetResource()->D3DResource.Get());
	}

	void D3D12DeferredRenderPipeline::ResizeSSAOBuffer()
	{
		mSSAOTexture = std::make_unique<RenderTarget2D>(mRHI, false, mTargetWight, mTargetHeight, DXGI_FORMAT_R16_UNORM, Colors::Black);
		mSSAOTexture->GetResource()->D3DResource->SetName(L"SSAO Buffer");

		D3D12TextureInfo TextureInfo;
		TextureInfo.Type = IMAGE_TYPE::IMAGE_TYPE_2D;
		TextureInfo.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		TextureInfo.Width = mTargetWight;
		TextureInfo.Height = mTargetHeight;
		TextureInfo.Depth = 1;
		TextureInfo.ArraySize = 1;
		TextureInfo.MipCount = 1;
		TextureInfo.Format = DXGI_FORMAT_R16_UNORM;
		TextureInfo.InitState = D3D12_RESOURCE_STATE_COMMON;
		mBlur1SSAOTexture = mRHI->CreateTexture(TextureInfo, TexCreate_SRV | TexCreate_UAV);
		mBlur2SSAOTexture = mRHI->CreateTexture(TextureInfo, TexCreate_SRV | TexCreate_UAV);

		auto weights = CalcGaussWeights(2.5f);
		int blurRadius = (int)weights.size() / 2;
		BlurSettingsConstants constants;
		constants.gBlurRadius = blurRadius;
		memcpy(constants.w, weights.data(), weights.size() * sizeof(float));

		mSSAOBlurCBRef = mRHI->CreateConstantBuffer(&constants, sizeof(constants));
	}

	void D3D12DeferredRenderPipeline::ResizeTAABuffer()
	{
		mVelocityBuffer = std::make_unique<RenderTarget2D>(mRHI, false, mTargetWight, mTargetHeight, DXGI_FORMAT_R16G16_FLOAT);
		mVelocityBuffer->GetResource()->D3DResource->SetName(L"TAA VelocityBuffer");

		D3D12TextureInfo TextureInfo;
		TextureInfo.Type = IMAGE_TYPE::IMAGE_TYPE_2D;
		TextureInfo.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		TextureInfo.Width = mTargetWight;
		TextureInfo.Height = mTargetHeight;
		TextureInfo.Depth = 1;
		TextureInfo.MipCount = 1;
		TextureInfo.ArraySize = 1;
		TextureInfo.InitState = D3D12_RESOURCE_STATE_COMMON;
		TextureInfo.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

		mPrevColorTexture = mRHI->CreateTexture(TextureInfo, TexCreate_SRV);
		mPrevColorTexture->GetD3DResource()->SetName(L"PrevColorBuffer");

		mCacheColorTexture = mRHI->CreateTexture(TextureInfo, TexCreate_SRV);
		mCacheColorTexture->GetD3DResource()->SetName(L"CacheColorBuffer");
	}

	void D3D12DeferredRenderPipeline::DrawCallScreen(ID3D12GraphicsCommandList* commandList)
	{
		Mesh* load_default_mesh_resource = AssetManager::GetInstance()->LoadDefaultMeshResource(DefaultMesh_ScreenQuad);
		ASSERT(load_default_mesh_resource != nullptr);
		SubMesh& refSubMesh = load_default_mesh_resource->Meshes[0];
		SubMeshResource subMesh;
		subMesh.VertexByteStride = sizeof(Vertex);
		subMesh.VertexBufferByteSize = subMesh.VertexByteStride * refSubMesh.Vertices.size();
		subMesh.IndexFormat = DXGI_FORMAT_R32_UINT;
		subMesh.IndexBufferByteSize = sizeof(unsigned) * refSubMesh.Indices.size();
		subMesh.PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		subMesh.VertexBufferRef = mRenderResource->GetMeshVertexBufferRef(AssetManager::GetInstance()->GetDefaultAssetGuid(DefaultMesh_ScreenQuad));
		subMesh.IndexBufferRef = mRenderResource->GetMeshIndexBufferRef(AssetManager::GetInstance()->GetDefaultAssetGuid(DefaultMesh_ScreenQuad));

		subMesh.IndexCount = refSubMesh.Indices.size();
		subMesh.StartIndexLocation = 0;
		subMesh.BaseVertexLocation = 0;

		mRHI->SetVertexBuffer(subMesh.VertexBufferRef, 0, subMesh.VertexByteStride, subMesh.VertexBufferByteSize);
		mRHI->SetIndexBuffer(subMesh.IndexBufferRef, 0, subMesh.IndexFormat, subMesh.IndexBufferByteSize);

		commandList->IASetPrimitiveTopology(subMesh.PrimitiveType);

		commandList->DrawIndexedInstanced(subMesh.IndexCount, 1, subMesh.StartIndexLocation, subMesh.BaseVertexLocation, 0);
		++RenderSystem::GetInstance()->mDrawCall;
	}

	void D3D12DeferredRenderPipeline::GenerateDirectionalShadowMap(const ShadowParameter& shadowParameter)
	{
		auto commandList = mRHI->GetDevice()->GetCommandList();

		auto& shadowMap = mShadowMap2D[mCurrentShadowMap2DIndex];

		if (shadowMap == nullptr || shadowMap->GetShadowMapSize() != shadowParameter.ShadowMapSize)
		{
			shadowMap = std::make_unique<ShadowMap2D>(shadowParameter.ShadowMapSize, mRHI);
		}

		mRHI->TransitionResource(shadowMap->GetTarget()->GetResource(), D3D12_RESOURCE_STATE_DEPTH_WRITE);

		commandList->RSSetViewports(1, &shadowMap->GetViewport());
		commandList->RSSetScissorRects(1, &shadowMap->GetScissorRect());
		
		commandList->ClearDepthStencilView(shadowMap->GetTarget()->GetDSV()->GetDescriptorHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		commandList->OMSetRenderTargets(0, nullptr, true, &shadowMap->GetTarget()->GetDSV()->GetDescriptorHandle());

		const auto& batchs = mRenderResource->GetBasePassBatchs();

		Shader* shader = mSingleShadowMapPSODescriptor.mShader;

		ID3D12PipelineState* pipeline_state = mRenderResource->GetPSO(mSingleShadowMapPSODescriptor);
		commandList->SetPipelineState(pipeline_state);
		commandList->SetGraphicsRootSignature(shader->mRootSignature.Get());

		DirectionalShadowParameter cbGPara;
		cbGPara.ViewProj = shadowParameter.ViewProj;

		ShadowPassCBRef = mRHI->CreateConstantBuffer(&cbGPara, sizeof(cbGPara));

		for (const auto& [materialResource, subMeshResource, modleResource, InstanceCount] : batchs)
		{
			shader->SetParameter("gInstanceData", modleResource.gInstanceDataD3D12StructuredBufferRef->GetSRV());
			shader->SetParameter("gBoneTransforms", modleResource.gBoneTransformsD3D12StructuredBufferRef->GetSRV());
			shader->SetParameter("cbLightView", ShadowPassCBRef);
			shader->BindParameters();

			mRHI->SetVertexBuffer(subMeshResource.VertexBufferRef, 0, subMeshResource.VertexByteStride, subMeshResource.VertexBufferByteSize);
			mRHI->SetIndexBuffer(subMeshResource.IndexBufferRef, 0, subMeshResource.IndexFormat, subMeshResource.IndexBufferByteSize);

			commandList->IASetPrimitiveTopology(subMeshResource.PrimitiveType);

			commandList->DrawIndexedInstanced(subMeshResource.IndexCount, InstanceCount, subMeshResource.StartIndexLocation, subMeshResource.BaseVertexLocation, 0);
			++RenderSystem::GetInstance()->mDrawCall;
		}

		mRHI->TransitionResource(shadowMap->GetTarget()->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
	}

	void D3D12DeferredRenderPipeline::GenerateOmnidirectionalShadowMap(ShadowParameter& shadowParameter)
	{
		auto commandList = mRHI->GetDevice()->GetCommandList();
		
		auto& shadowMap = mShadowMapCube[mCurrentShadowMapCubeIndex];

		if (shadowMap == nullptr || shadowMap->GetShadowMapSize() != shadowParameter.ShadowMapSize)
		{
			shadowMap = std::make_unique<ShadowMapCube>(shadowParameter.ShadowMapSize, mRHI);
		}

		mRHI->TransitionResource(shadowMap->GetTarget()->GetResource(), D3D12_RESOURCE_STATE_DEPTH_WRITE);

		commandList->RSSetViewports(1, &shadowMap->GetViewport());
		commandList->RSSetScissorRects(1, &shadowMap->GetScissorRect());

		shadowMap->SetView(shadowParameter.LightPosition, 0.001f, shadowParameter.LightRange);

		OmnidirectionalShadowParameter cbGPara;
		for (int i = 0; i < 6; ++i)
		{
			cbGPara.View[i] = shadowMap->GetSceneView(i);
		}
		cbGPara.Proj = shadowMap->GetSceneProj();
		cbGPara.LightPosition = shadowParameter.LightPosition;
		cbGPara.LightRange = shadowParameter.LightRange;

		commandList->ClearDepthStencilView(shadowMap->GetTarget()->GetDSV(6)->GetDescriptorHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		commandList->OMSetRenderTargets(0, nullptr, true, &shadowMap->GetTarget()->GetDSV(6)->GetDescriptorHandle());

		const auto& batchs = mRenderResource->GetBasePassBatchs();

		Shader* shader = mOmnidirectionalShadowMapPSODescriptor.mShader;

		ID3D12PipelineState* pipeline_state = mRenderResource->GetPSO(mOmnidirectionalShadowMapPSODescriptor);
		commandList->SetPipelineState(pipeline_state);
		commandList->SetGraphicsRootSignature(shader->mRootSignature.Get());

		ShadowPassCBRef = mRHI->CreateConstantBuffer(&cbGPara, sizeof(cbGPara));

		for (const auto& [materialResource, subMeshResource, modleResource, InstanceCount] : batchs)
		{
			shader->SetParameter("gInstanceData", modleResource.gInstanceDataD3D12StructuredBufferRef->GetSRV());
			shader->SetParameter("gBoneTransforms", modleResource.gBoneTransformsD3D12StructuredBufferRef->GetSRV());
			shader->SetParameter("cbLightView", ShadowPassCBRef);
			shader->BindParameters();

			mRHI->SetVertexBuffer(subMeshResource.VertexBufferRef, 0, subMeshResource.VertexByteStride, subMeshResource.VertexBufferByteSize);
			mRHI->SetIndexBuffer(subMeshResource.IndexBufferRef, 0, subMeshResource.IndexFormat, subMeshResource.IndexBufferByteSize);

			commandList->IASetPrimitiveTopology(subMeshResource.PrimitiveType);

			commandList->DrawIndexedInstanced(subMeshResource.IndexCount, InstanceCount, subMeshResource.StartIndexLocation, subMeshResource.BaseVertexLocation, 0);
			++RenderSystem::GetInstance()->mDrawCall;
		}

		mRHI->TransitionResource(shadowMap->GetTarget()->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
	}
	
	void D3D12DeferredRenderPipeline::CreateSceneCaptureCube()
	{
		mIBLEnvironmentMap = std::make_unique<SceneCaptureCube>(false, 2048, DXGI_FORMAT_R32G32B32A32_FLOAT, mRHI);

		mIBLEnvironmentMap->CreatePerspectiveViews({ 0.0f, 0.0f, 0.0f }, 0.1f, 10.0f);
		std::shared_ptr<TextureData> texture = LoadHDRTextureForFile(EngineFileSystem::GetInstance()->GetActualPath("Resource/Texture/Newport_Loft_Ref.hdr"));
		D3D12TextureInfo info;
		info.Type = texture->Info.mType;
		info.Format = D3D12Texture::TransformationToD3DFormat(texture->Info.mFormat);
		info.Width = texture->Info.mWidth;
		info.Height = texture->Info.mHeight;
		// TODO:纹理类型记得扩展
		info.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		info.Depth = texture->Info.mDepth;
		info.ArraySize = texture->Info.mArrayLayers;
		info.MipCount = texture->Info.mMipLevels;

		mIBLEquirectangularTexture = mRHI->CreateTexture(info, TexCreate_SRV);

		mRHI->UploadTextureData(mIBLEquirectangularTexture, texture.get());

		mIBLIrradianceMap = std::make_unique<SceneCaptureCube>(false, 32, DXGI_FORMAT_R32G32B32A32_FLOAT, mRHI);
		mIBLIrradianceMap->CreatePerspectiveViews({ 0.0f, 0.0f, 0.0f }, 0.1f, 10.0f);

		for (uint32_t i = 0; i < mIBLPrefilterMaxMipLevel; ++i)
		{
			UINT MipWidth = UINT(128 * std::pow(0.5, i));
			auto PrefilterEnvMap = std::make_unique<SceneCaptureCube>(false, MipWidth, DXGI_FORMAT_R16G16B16A16_FLOAT, mRHI);
			PrefilterEnvMap->CreatePerspectiveViews({ 0.0f, 0.0f, 0.0f }, 0.1f, 10.0f);

			mIBLPrefilterMap.push_back(std::move(PrefilterEnvMap));
		}

		std::shared_ptr<TextureData> lutTexture = LoadTextureForFile(EngineFileSystem::GetInstance()->GetActualPath("Resource/Texture/IBL_BRDF_LUT.png"), false);
		info.Type = lutTexture->Info.mType;
		info.Format = D3D12Texture::TransformationToD3DFormat(lutTexture->Info.mFormat);
		info.Width = lutTexture->Info.mWidth;
		info.Height = lutTexture->Info.mHeight;
		// TODO:纹理类型记得扩展
		info.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		info.Depth = lutTexture->Info.mDepth;
		info.ArraySize = lutTexture->Info.mArrayLayers;
		info.MipCount = lutTexture->Info.mMipLevels;

		mBRDFLUTTexture = mRHI->CreateTexture(info, TexCreate_SRV);
	}
	
	
	void D3D12DeferredRenderPipeline::CreateIBLEnvironmentMap()
	{
		for (int i = 0; i < 6; ++i)
		{
			Matrix4x4 View = mIBLEnvironmentMap->GetSceneView(i).View;
			Matrix4x4 Proj = mIBLEnvironmentMap->GetSceneView(i).Proj;

			EnvironmentConstant PassCB;
			PassCB.View = View;
			PassCB.Proj = Proj;

			mIBLEnvironmentPassCBRef[i] = mRHI->CreateConstantBuffer(&PassCB, sizeof(PassCB));
		}

		ID3D12GraphicsCommandList* commandList = mRHI->GetDevice()->GetCommandList();

		mRHI->TransitionResource(mIBLEnvironmentMap->GetRTCube()->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);

		commandList->RSSetViewports(1, &mIBLEnvironmentMap->GetViewport());
		commandList->RSSetScissorRects(1, &mIBLEnvironmentMap->GetScissorRect());

		commandList->SetPipelineState(mRenderResource->GetPSO(mIBLEnvironmentPSODescriptor));

		Shader* shader = mIBLEnvironmentPSODescriptor.mShader;
		commandList->SetGraphicsRootSignature(shader->mRootSignature.Get());

		Mesh* boxMesh = AssetManager::GetInstance()->LoadDefaultMeshResource(DefaultMesh_Box);
		SubMeshResource subMesh;
		subMesh.VertexByteStride = sizeof(Vertex);
		subMesh.VertexBufferByteSize = (uint32_t)(subMesh.VertexByteStride * boxMesh->Meshes[0].Vertices.size());
		subMesh.IndexFormat = DXGI_FORMAT_R32_UINT;
		subMesh.IndexBufferByteSize = (uint32_t)(sizeof(unsigned) * boxMesh->Meshes[0].Indices.size());
		subMesh.PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		subMesh.VertexBufferRef = mRenderResource->GetMeshVertexBufferRef(AssetManager::GetInstance()->GetDefaultAssetGuid(DefaultMesh_Box));
		subMesh.IndexBufferRef = mRenderResource->GetMeshIndexBufferRef(AssetManager::GetInstance()->GetDefaultAssetGuid(DefaultMesh_Box));

		subMesh.IndexCount = (uint32_t)boxMesh->Meshes[0].Indices.size();
		subMesh.StartIndexLocation = 0;
		subMesh.BaseVertexLocation = 0;
		
		for (int i = 0; i < 6; ++i)
		{
			std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvDescriptors;
			auto rtv = mIBLEnvironmentMap->GetRTCube()->GetRTV(i);
			
			commandList->ClearRenderTargetView(rtv->GetDescriptorHandle(), mIBLEnvironmentMap->GetRTCube()->GetClearColorPtr(), 0, nullptr);
			commandList->OMSetRenderTargets(1, &rtv->GetDescriptorHandle(), true, nullptr);

			shader->SetParameter("cbEnvironmentPass", mIBLEnvironmentPassCBRef[i]);
			shader->SetParameter("EquirectangularMap", mIBLEquirectangularTexture->GetSRV());

			shader->BindParameters();

			mRHI->SetVertexBuffer(subMesh.VertexBufferRef, 0, subMesh.VertexByteStride, subMesh.VertexBufferByteSize);
			mRHI->SetIndexBuffer(subMesh.IndexBufferRef, 0, subMesh.IndexFormat, subMesh.IndexBufferByteSize);

			commandList->IASetPrimitiveTopology(subMesh.PrimitiveType);

			commandList->DrawIndexedInstanced(subMesh.IndexCount, 1, subMesh.StartIndexLocation, subMesh.BaseVertexLocation, 0);
		}

		mRHI->TransitionResource(mIBLEnvironmentMap->GetRTCube()->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
	}

	void D3D12DeferredRenderPipeline::CreateIBLIrradianceMap()
	{
		for (int i = 0; i < 6; ++i)
		{
			Matrix4x4 View = mIBLIrradianceMap->GetSceneView(i).View;
			Matrix4x4 Proj = mIBLIrradianceMap->GetSceneView(i).Proj;

			EnvironmentConstant PassCB;
			PassCB.View = View;
			PassCB.Proj = Proj;

			mIBLIrradiancePassCBRef[i] = mRHI->CreateConstantBuffer(&PassCB, sizeof(PassCB));
		}

		ID3D12GraphicsCommandList* commandList = mRHI->GetDevice()->GetCommandList();

		mRHI->TransitionResource(mIBLIrradianceMap->GetRTCube()->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);

		commandList->RSSetViewports(1, &mIBLIrradianceMap->GetViewport());
		commandList->RSSetScissorRects(1, &mIBLIrradianceMap->GetScissorRect());

		commandList->SetPipelineState(mRenderResource->GetPSO(mIBLIrradiancePSODescriptor));

		Shader* shader = mIBLIrradiancePSODescriptor.mShader;
		commandList->SetGraphicsRootSignature(shader->mRootSignature.Get());

		Mesh* boxMesh = AssetManager::GetInstance()->LoadDefaultMeshResource(DefaultMesh_Box);
		SubMeshResource subMesh;
		subMesh.VertexByteStride = sizeof(Vertex);
		subMesh.VertexBufferByteSize = (uint32_t)(subMesh.VertexByteStride * boxMesh->Meshes[0].Vertices.size());
		subMesh.IndexFormat = DXGI_FORMAT_R32_UINT;
		subMesh.IndexBufferByteSize = (uint32_t)(sizeof(unsigned) * boxMesh->Meshes[0].Indices.size());
		subMesh.PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		subMesh.VertexBufferRef = mRenderResource->GetMeshVertexBufferRef(AssetManager::GetInstance()->GetDefaultAssetGuid(DefaultMesh_Box));
		subMesh.IndexBufferRef = mRenderResource->GetMeshIndexBufferRef(AssetManager::GetInstance()->GetDefaultAssetGuid(DefaultMesh_Box));

		subMesh.IndexCount = (uint32_t)boxMesh->Meshes[0].Indices.size();
		subMesh.StartIndexLocation = 0;
		subMesh.BaseVertexLocation = 0;

		for (int i = 0; i < 6; ++i)
		{
			std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvDescriptors;
			auto rtv = mIBLIrradianceMap->GetRTCube()->GetRTV(i);

			commandList->ClearRenderTargetView(rtv->GetDescriptorHandle(), mIBLIrradianceMap->GetRTCube()->GetClearColorPtr(), 0, nullptr);
			commandList->OMSetRenderTargets(1, &rtv->GetDescriptorHandle(), true, nullptr);

			shader->SetParameter("cbEnvironmentPass", mIBLIrradiancePassCBRef[i]);
			shader->SetParameter("SkyCubeTexture", mIBLEnvironmentMap->GetRTCube()->GetSRV());

			shader->BindParameters();

			mRHI->SetVertexBuffer(subMesh.VertexBufferRef, 0, subMesh.VertexByteStride, subMesh.VertexBufferByteSize);
			mRHI->SetIndexBuffer(subMesh.IndexBufferRef, 0, subMesh.IndexFormat, subMesh.IndexBufferByteSize);

			commandList->IASetPrimitiveTopology(subMesh.PrimitiveType);

			commandList->DrawIndexedInstanced(subMesh.IndexCount, 1, subMesh.StartIndexLocation, subMesh.BaseVertexLocation, 0);
		}

		mRHI->TransitionResource(mIBLIrradianceMap->GetRTCube()->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
	}

	void D3D12DeferredRenderPipeline::CreateIBLPrefilterEnvironmentMap()
	{
		for (UINT Mip = 0; Mip < mIBLPrefilterMaxMipLevel; Mip++)
		{
			float Roughness = (float)Mip / (float)(mIBLPrefilterMaxMipLevel - 1);

			for (UINT i = 0; i < 6; i++)
			{
				Matrix4x4 View = mIBLPrefilterMap[Mip]->GetSceneView(i).View;
				Matrix4x4 Proj = mIBLPrefilterMap[Mip]->GetSceneView(i).Proj;

				EnvironmentConstant PassCB;
				PassCB.View = View;
				PassCB.Proj = Proj;
				PassCB.Roughness = Roughness;

				mIBLPrefilterPassCBRef[Mip * 6 + i] = mRHI->CreateConstantBuffer(&PassCB, sizeof(PassCB));
			}
		}

		ID3D12GraphicsCommandList* commandList = mRHI->GetDevice()->GetCommandList();

		for (UINT Mip = 0; Mip < mIBLPrefilterMaxMipLevel; Mip++)
		{
			mRHI->TransitionResource(mIBLPrefilterMap[Mip]->GetRTCube()->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);

			commandList->RSSetViewports(1, &mIBLPrefilterMap[Mip]->GetViewport());
			commandList->RSSetScissorRects(1, &mIBLPrefilterMap[Mip]->GetScissorRect());

			commandList->SetPipelineState(mRenderResource->GetPSO(mIBLPrefilterPSODescriptor));

			Shader* shader = mIBLPrefilterPSODescriptor.mShader;
			commandList->SetGraphicsRootSignature(shader->mRootSignature.Get());

			Mesh* boxMesh = AssetManager::GetInstance()->LoadDefaultMeshResource(DefaultMesh_Box);
			SubMeshResource subMesh;
			subMesh.VertexByteStride = sizeof(Vertex);
			subMesh.VertexBufferByteSize = (uint32_t)(subMesh.VertexByteStride * boxMesh->Meshes[0].Vertices.size());
			subMesh.IndexFormat = DXGI_FORMAT_R32_UINT;
			subMesh.IndexBufferByteSize = (uint32_t)(sizeof(unsigned) * boxMesh->Meshes[0].Indices.size());
			subMesh.PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

			subMesh.VertexBufferRef = mRenderResource->GetMeshVertexBufferRef(AssetManager::GetInstance()->GetDefaultAssetGuid(DefaultMesh_Box));
			subMesh.IndexBufferRef = mRenderResource->GetMeshIndexBufferRef(AssetManager::GetInstance()->GetDefaultAssetGuid(DefaultMesh_Box));

			subMesh.IndexCount = (uint32_t)boxMesh->Meshes[0].Indices.size();
			subMesh.StartIndexLocation = 0;
			subMesh.BaseVertexLocation = 0;

			for (int i = 0; i < 6; ++i)
			{
				std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvDescriptors;
				auto rtv = mIBLPrefilterMap[Mip]->GetRTCube()->GetRTV(i);

				commandList->ClearRenderTargetView(rtv->GetDescriptorHandle(), mIBLPrefilterMap[Mip]->GetRTCube()->GetClearColorPtr(), 0, nullptr);
				commandList->OMSetRenderTargets(1, &rtv->GetDescriptorHandle(), true, nullptr);

				shader->SetParameter("cbEnvironmentPass", mIBLPrefilterPassCBRef[Mip * 6 + i]);
				shader->SetParameter("SkyCubeTexture", mIBLEnvironmentMap->GetRTCube()->GetSRV());

				shader->BindParameters();

				mRHI->SetVertexBuffer(subMesh.VertexBufferRef, 0, subMesh.VertexByteStride, subMesh.VertexBufferByteSize);
				mRHI->SetIndexBuffer(subMesh.IndexBufferRef, 0, subMesh.IndexFormat, subMesh.IndexBufferByteSize);

				commandList->IASetPrimitiveTopology(subMesh.PrimitiveType);

				commandList->DrawIndexedInstanced(subMesh.IndexCount, 1, subMesh.StartIndexLocation, subMesh.BaseVertexLocation, 0);
			}

			mRHI->TransitionResource(mIBLPrefilterMap[Mip]->GetRTCube()->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
		}
	}

	/*
	void D3D12DeferredRenderPipeline::CreateIBLEnviromentTexture()
	{
		std::array<std::shared_ptr<TextureData>, 6> datas;
		
		datas[0] = LoadHDRTextureForFile(EngineFileSystem::GetInstance()->GetActualPath("Resource/Texture/y/X+.hdr"));
		datas[1] = LoadHDRTextureForFile(EngineFileSystem::GetInstance()->GetActualPath("Resource/Texture/y/X-.hdr"));
		datas[2] = LoadHDRTextureForFile(EngineFileSystem::GetInstance()->GetActualPath("Resource/Texture/y/Y+.hdr"));
		datas[3] = LoadHDRTextureForFile(EngineFileSystem::GetInstance()->GetActualPath("Resource/Texture/y/Y-.hdr"));
		datas[4] = LoadHDRTextureForFile(EngineFileSystem::GetInstance()->GetActualPath("Resource/Texture/y/Z+.hdr"));
		datas[5] = LoadHDRTextureForFile(EngineFileSystem::GetInstance()->GetActualPath("Resource/Texture/y/Z-.hdr"));
		
		D3D12TextureInfo info;
		info.Type = IMAGE_TYPE::IMAGE_TYPE_CUBE;
		info.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		info.Width = datas[0]->Info.mWidth;
		info.Height = datas[0]->Info.mHeight;
		info.ArraySize = 6;
		info.MipCount = datas[0]->Info.mMipLevels;
		info.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		
		mIBLEnviromentTexture = mRHI->CreateTexture(info, TexCreate_SRV);

		std::array<const TextureData*, 6> up;
		for (int i = 0; i < 6; ++i)
		{
			up[i] = datas[i].get();
		}

		mRHI->UploadCubeTextureData(mIBLEnviromentTexture, up);
	}
	*/
}
