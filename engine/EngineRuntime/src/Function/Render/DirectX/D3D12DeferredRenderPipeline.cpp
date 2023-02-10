#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Core/Math/Color.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12RHI.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12DeferredRenderPipeline.h"

#include "EngineRuntime/include/EngineRuntime.h"
#include "EngineRuntime/include/Function/Render/RenderSystem.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12Shader.h"
#include "EngineRuntime/include/Resource/AssetManager/AssetManager.h"

namespace Engine
{
	D3D12DeferredRenderPipeline::D3D12DeferredRenderPipeline(D3D12RHI* rhi, D3D12RenderResource* resource)
		:mRHI(rhi), mRenderResource(resource)
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

		if (EngineRuntime::GetInstance()->rdoc_api != nullptr && RenderSystem::GetInstance()->mFrameCount == 0)
		{
			EngineRuntime::GetInstance()->rdoc_api->StartFrameCapture(NULL, NULL);
		}

		if (mImGuiDevice != nullptr)
		{
			mImGuiDevice->DrawUI();
		}
		
		/*
		* 场景渲染
		*/
		SetDescriptorHeaps();

		/*
		if (RenderSystem::GetInstance()->mFrameCount == 0)
		{
			CreateIBLEnviromentMap();
		}
		*/

		UploadResource();

		DepthPrePass();

		BasePass();

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

		if (EngineRuntime::GetInstance()->rdoc_api != nullptr && RenderSystem::GetInstance()->mFrameCount == 0)
		{
			EngineRuntime::GetInstance()->rdoc_api->EndFrameCapture(NULL, NULL);
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

		ResizeTAABuffer();
	}

	void D3D12DeferredRenderPipeline::UploadResource()
	{
		mRenderResource->UpdateRenderResource();
	}

	void D3D12DeferredRenderPipeline::CreateD3D12State()
	{
		mRHI->ResetCommandAllocator();
		mRHI->ResetCommandList();

		mRenderResource->UploadDefaultVertexResource();

		//CreateSceneCaptureCube();
		CreateIBLEnviromentTexture();
		CreateGlobalShaders();
		CreateGlobalPSO();

		mRHI->ExecuteCommandLists();
		mRHI->FlushCommandQueue();
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

		/*
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
		*/

		{
			ShaderInfo shaderInfo;
			std::shared_ptr<Blob> vsBlob = EngineFileSystem::GetInstance()->ReadFile("Shaders/BasePassSkyVS.cso");
			std::shared_ptr<Blob> psBlob = EngineFileSystem::GetInstance()->ReadFile("Shaders/BasePassSkyPS.cso");
			if (vsBlob == nullptr || psBlob == nullptr)
			{
				LOG_FATAL("IBLEnvironmentShader文件读取出错");
			}
			shaderInfo.mVertexShader = vsBlob;
			shaderInfo.mPixelShader = psBlob;
			mSkyBoxPassShader = std::make_unique<Shader>(shaderInfo, mRHI);
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
	}

	void D3D12DeferredRenderPipeline::CreateGlobalPSO()
	{
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

		/*
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
		*/

		{
			mSkyBoxPassPSODescriptor.mInputLayoutName = "BaseInputLayout";
			mSkyBoxPassPSODescriptor.mShader = mSkyBoxPassShader.get();
			mSkyBoxPassPSODescriptor.mPrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			mSkyBoxPassPSODescriptor.mRTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
			mSkyBoxPassPSODescriptor.mRTVFormats[1] = DXGI_FORMAT_R8G8B8A8_SNORM;
			mSkyBoxPassPSODescriptor.mRTVFormats[2] = DXGI_FORMAT_R32G32B32A32_FLOAT;
			mSkyBoxPassPSODescriptor.mRTVFormats[3] = DXGI_FORMAT_R8G8B8A8_UNORM;
			mSkyBoxPassPSODescriptor.mRTVFormats[4] = DXGI_FORMAT_R16G16_FLOAT;
			mSkyBoxPassPSODescriptor.mNumRenderTargets = 5;
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
			mDeferredLightingPSODescriptor.mInputLayoutName = "BaseInputLayout";
			mDeferredLightingPSODescriptor.m4xMsaaState = false;
			mDeferredLightingPSODescriptor.mDepthStencilDesc.DepthEnable = false;
			mDeferredLightingPSODescriptor.mPrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			mDeferredLightingPSODescriptor.mRTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
			mDeferredLightingPSODescriptor.mNumRenderTargets = 1;
			mDeferredLightingPSODescriptor.mRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
			mDeferredLightingPSODescriptor.mRasterizerDesc.DepthClipEnable = false;
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
			
		for (const auto& [materialResource, subMeshResource, cD3D12StructuredBufferRef, InstanceCount] : batchs)
		{
			mZPrePassShader->SetParameter("gInstanceData", cD3D12StructuredBufferRef->GetSRV());
			mZPrePassShader->SetParameter("cbPass", cbPassRef);
			mZPrePassShader->BindParameters();

			mRHI->SetVertexBuffer(subMeshResource.VertexBufferRef, 0, subMeshResource.VertexByteStride, subMeshResource.VertexBufferByteSize);
			mRHI->SetIndexBuffer(subMeshResource.IndexBufferRef, 0, subMeshResource.IndexFormat, subMeshResource.IndexBufferByteSize);

			commandList->IASetPrimitiveTopology(subMeshResource.PrimitiveType);

			commandList->DrawIndexedInstanced(subMeshResource.IndexCount, InstanceCount, subMeshResource.StartIndexLocation, subMeshResource.BaseVertexLocation, 0);
		}
	}

	void D3D12DeferredRenderPipeline::BasePass()
	{
		auto commandList = mRHI->GetDevice()->GetCommandList();

		mRHI->TransitionResource(mPositionGBuffer->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		mRHI->TransitionResource(mNormalGBuffer->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		mRHI->TransitionResource(mColorGBuffer->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		mRHI->TransitionResource(mMetallicGBuffer->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		mRHI->TransitionResource(mVelocityBuffer->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);

		commandList->ClearRenderTargetView(mPositionGBuffer->GetRTV()->GetDescriptorHandle(), mPositionGBuffer->GetClearColorPtr(), 0, nullptr);
		commandList->ClearRenderTargetView(mNormalGBuffer->GetRTV()->GetDescriptorHandle(), mNormalGBuffer->GetClearColorPtr(), 0, nullptr);
		commandList->ClearRenderTargetView(mColorGBuffer->GetRTV()->GetDescriptorHandle(), mColorGBuffer->GetClearColorPtr(), 0, nullptr);
		commandList->ClearRenderTargetView(mMetallicGBuffer->GetRTV()->GetDescriptorHandle(), mMetallicGBuffer->GetClearColorPtr(), 0, nullptr);
		commandList->ClearRenderTargetView(mVelocityBuffer->GetRTV()->GetDescriptorHandle(), mVelocityBuffer->GetClearColorPtr(), 0, nullptr);

		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvDescriptors;
		rtvDescriptors.push_back(mPositionGBuffer->GetRTV()->GetDescriptorHandle());
		rtvDescriptors.push_back(mNormalGBuffer->GetRTV()->GetDescriptorHandle());
		rtvDescriptors.push_back(mColorGBuffer->GetRTV()->GetDescriptorHandle());
		rtvDescriptors.push_back(mMetallicGBuffer->GetRTV()->GetDescriptorHandle());
		rtvDescriptors.push_back(mVelocityBuffer->GetRTV()->GetDescriptorHandle());

		auto descriptorCache = mRHI->GetDevice()->GetCommandContext()->GetDescriptorCache();
		CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle = descriptorCache->AppendRtvDescriptors(rtvDescriptors);

		commandList->OMSetRenderTargets(5, &cpuHandle, true, &DepthStencilView());

		const auto& batchs = mRenderResource->GetBasePassBatchs();

		for (const auto & [materialResource, subMeshResource, cD3D12StructuredBufferRef, InstanceCount] : batchs)
		{
			const GraphicsPSODescriptor& Descriptor = materialResource.Descriptor;
			ID3D12PipelineState* pipeline_state = mRenderResource->GetPSO(Descriptor);
			ASSERT(pipeline_state != nullptr);
			commandList->SetPipelineState(pipeline_state);

			commandList->SetGraphicsRootSignature(Descriptor.mShader->mRootSignature.Get());

			materialResource.BindShaderBindParameters(cD3D12StructuredBufferRef);

			mRHI->SetVertexBuffer(subMeshResource.VertexBufferRef, 0, subMeshResource.VertexByteStride, subMeshResource.VertexBufferByteSize);
			mRHI->SetIndexBuffer(subMeshResource.IndexBufferRef, 0, subMeshResource.IndexFormat, subMeshResource.IndexBufferByteSize);

			commandList->IASetPrimitiveTopology(subMeshResource.PrimitiveType);

			commandList->DrawIndexedInstanced(subMeshResource.IndexCount, InstanceCount, subMeshResource.StartIndexLocation, subMeshResource.BaseVertexLocation, 0);
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
			//mSkyBoxPassPSODescriptor.mShader->SetParameter("SkyCubeTexture", mIBLEnvironmentMap->GetRTCube()->GetSRV());
			mSkyBoxPassPSODescriptor.mShader->SetParameter("SkyCubeTexture", mIBLEnviromentTexture->GetSRV());
			mSkyBoxPassPSODescriptor.mShader->BindParameters();
			
			mRHI->SetVertexBuffer(subMesh.VertexBufferRef, 0, subMesh.VertexByteStride, subMesh.VertexBufferByteSize);
			mRHI->SetIndexBuffer(subMesh.IndexBufferRef, 0, subMesh.IndexFormat, subMesh.IndexBufferByteSize);

			commandList->IASetPrimitiveTopology(subMesh.PrimitiveType);

			commandList->DrawIndexedInstanced(subMesh.IndexCount, 1, subMesh.StartIndexLocation, subMesh.BaseVertexLocation, 0);
		}
		
		mRHI->TransitionResource(mPositionGBuffer->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
		mRHI->TransitionResource(mNormalGBuffer->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
		mRHI->TransitionResource(mColorGBuffer->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
		mRHI->TransitionResource(mMetallicGBuffer->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
		mRHI->TransitionResource(mVelocityBuffer->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
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

		mDeferredLightingPSODescriptor.mShader->SetParameter("PositionGBuffer", mPositionGBuffer->GetSRV());
		mDeferredLightingPSODescriptor.mShader->SetParameter("NormalGBuffer", mNormalGBuffer->GetSRV());
		mDeferredLightingPSODescriptor.mShader->SetParameter("ColorGBuffer", mColorGBuffer->GetSRV());
		mDeferredLightingPSODescriptor.mShader->SetParameter("MetallicGBuffer", mMetallicGBuffer->GetSRV());
		
		mDeferredLightingPSODescriptor.mShader->SetParameter("cbPass", mRenderResource->GetCbPassRef());
		if (mRenderResource->mDirectionalLightStructuredBufferRef != nullptr)
		{
			mDeferredLightingPSODescriptor.mShader->SetParameter("DirectionalLightList", mRenderResource->mDirectionalLightStructuredBufferRef->GetSRV());
		}
		if (mRenderResource->mPointLightStructuredBufferRef)
		{
			mDeferredLightingPSODescriptor.mShader->SetParameter("PointLightList", mRenderResource->mPointLightStructuredBufferRef->GetSRV());
		}
		if (mRenderResource->mSpotLightStructuredBufferRef)
		{
			mDeferredLightingPSODescriptor.mShader->SetParameter("SpotLightList", mRenderResource->mSpotLightStructuredBufferRef->GetSRV());
		}
		mDeferredLightingPSODescriptor.mShader->SetParameter("cbLightCommon", mRenderResource->mLightCommonDataBuffer);
		

		mDeferredLightingPSODescriptor.mShader->BindParameters();

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
		}

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
		}


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
		}

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
		mColorGBuffer = std::make_unique<RenderTarget2D>(mRHI, false, mTargetWight, mTargetHeight, DXGI_FORMAT_R32G32B32A32_FLOAT);
		mColorGBuffer->GetResource()->D3DResource->SetName(L"ColorGBuffer");
		mMetallicGBuffer = std::make_unique<RenderTarget2D>(mRHI, false, mTargetWight, mTargetHeight, DXGI_FORMAT_R8G8B8A8_UNORM);
		mMetallicGBuffer->GetResource()->D3DResource->SetName(L"MetallicGBuffer");

	}

	void D3D12DeferredRenderPipeline::ResizeRenderTarget()
	{
		mColorTexture = std::make_unique<RenderTarget2D>(mRHI, false, mTargetWight, mTargetHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, Colors::Black);
		mColorTexture->GetResource()->D3DResource->SetName(L"DeferredLighting");

		mSceneRenderTarget = std::make_unique<RenderTarget2D>(mRHI, false, mTargetWight, mTargetHeight, DXGI_FORMAT_R8G8B8A8_UNORM, Colors::Black);
		mSceneRenderTarget->GetResource()->D3DResource->SetName(L"测试 渲染到纹理");

		mImGuiDevice->SetSceneUITexture(mSceneRenderTarget->GetResource()->D3DResource.Get());
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

	/*
	void D3D12DeferredRenderPipeline::CreateSceneCaptureCube()
	{
		mIBLEnvironmentMap = std::make_unique<SceneCaptureCube>(false, 2048, DXGI_FORMAT_R32G32B32A32_FLOAT, mRHI);
		
		mIBLEnvironmentMap->CreatePerspectiveViews({ 0.0f, 0.0f, 0.0f }, 0.1f, 10.0f);
		std::shared_ptr<TextureData> texture = LoadHDRTextureForFile(EngineFileSystem::GetInstance()->GetActualPath("Resource/Texture/Shiodome_Stairs_3k.hdr"));
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

		mIBLEnviromentTexture = mRHI->CreateTexture(info, TexCreate_SRV);

		mRHI->UploadTextureData(mIBLEnviromentTexture, texture.get());
	}
	*/

	/*
	void D3D12DeferredRenderPipeline::CreateIBLEnviromentMap()
	{
		for (int i = 0; i < 6; ++i)
		{
			Matrix4x4 View = mIBLEnvironmentMap->GetSceneView(i).View;
			Matrix4x4 Proj = mIBLEnvironmentMap->GetSceneView(i).Proj;

			CameraPassConstants PassCB;
			PassCB.View = View.Transpose();
			PassCB.Proj = Proj.Transpose();

			mIBLEnviromentPassCBRef[i] = mRHI->CreateConstantBuffer(&PassCB, sizeof(PassCB));
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

			shader->SetParameter("cbPass", mIBLEnviromentPassCBRef[i]);
			shader->SetParameter("EquirectangularMap", mIBLEnviromentTexture->GetSRV());

			shader->BindParameters();

			mRHI->SetVertexBuffer(subMesh.VertexBufferRef, 0, subMesh.VertexByteStride, subMesh.VertexBufferByteSize);
			mRHI->SetIndexBuffer(subMesh.IndexBufferRef, 0, subMesh.IndexFormat, subMesh.IndexBufferByteSize);

			commandList->IASetPrimitiveTopology(subMesh.PrimitiveType);

			commandList->DrawIndexedInstanced(subMesh.IndexCount, 1, subMesh.StartIndexLocation, subMesh.BaseVertexLocation, 0);
		}

		mRHI->TransitionResource(mIBLEnvironmentMap->GetRTCube()->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
	}
	*/

	void D3D12DeferredRenderPipeline::CreateIBLEnviromentTexture()
	{
		std::array<std::shared_ptr<TextureData>, 6> datas;
		datas[0] = LoadHDRTextureForFile(EngineFileSystem::GetInstance()->GetActualPath("Resource/Texture/skybox_specular_X+.hdr"));
		datas[1] = LoadHDRTextureForFile(EngineFileSystem::GetInstance()->GetActualPath("Resource/Texture/skybox_specular_X-.hdr"));
		datas[2] = LoadHDRTextureForFile(EngineFileSystem::GetInstance()->GetActualPath("Resource/Texture/skybox_specular_Y+.hdr"));
		datas[3] = LoadHDRTextureForFile(EngineFileSystem::GetInstance()->GetActualPath("Resource/Texture/skybox_specular_X-.hdr"));
		datas[4] = LoadHDRTextureForFile(EngineFileSystem::GetInstance()->GetActualPath("Resource/Texture/skybox_specular_Z+.hdr"));
		datas[5] = LoadHDRTextureForFile(EngineFileSystem::GetInstance()->GetActualPath("Resource/Texture/skybox_specular_Z-.hdr"));

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
}
