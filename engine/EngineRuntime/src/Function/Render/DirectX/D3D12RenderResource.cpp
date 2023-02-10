#include "EngineRuntime/include/Function/Render/DirectX/D3D12RenderResource.h"
#include "EngineRuntime/include/Core/Math/Math.h"
#include "EngineRuntime/include/Function/Render/RenderSystem.h"
#include "EngineRuntime/include/Resource/AssetManager/AssetManager.h"

namespace Engine
{
	D3D12RenderResource::D3D12RenderResource(D3D12RHI* rhi)
		:mRHI(rhi)
	{
		mGraphicsPSOManager = std::make_unique<GraphicsPSOManager>(mRHI, &mInputLayoutManager);

		ShaderInfo shaderInfo;
		std::shared_ptr<Blob> vsBlob = EngineFileSystem::GetInstance()->ReadFile("Shaders/BasePassVS.cso");
		std::shared_ptr<Blob> psBlob = EngineFileSystem::GetInstance()->ReadFile("Shaders/BasePassPS.cso");
		if (vsBlob == nullptr || psBlob == nullptr)
		{
			LOG_FATAL("BaseShader文件读取出错");
		}
		shaderInfo.mVertexShader = vsBlob;
		shaderInfo.mPixelShader = psBlob;
		mDefaultBasePassShader = std::make_unique<Shader>(shaderInfo, mRHI);

		CreateInputLayout();
	}

	D3D12RenderResource::~D3D12RenderResource()
	{
		mGraphicsPSOManager = nullptr;
		mConstantMap.clear();
		mStructuredMap.clear();
		mRWStructuredMap.clear();
		mVertexMap.clear();
		mIndexMap.clear();
		mReadBackMap.clear();
	}

	void D3D12RenderResource::EndFrameBuffer()
	{
		RenderResource::EndFrameBuffer();
	}

	void D3D12RenderResource::UpdateRenderResource()
	{
		mBasePassBatchs.clear();

		// Update Camera

		mRenderCameraBuffer = mRHI->CreateConstantBuffer(&mMainCameraPassCB, sizeof(mMainCameraPassCB));


		// Update GameObject

		std::unordered_map<MaterialData*, std::unordered_map<SubMesh*, std::vector<ObjectConstants>>> cccc;

		for (auto& [refMesh, refMaterials, constant] : mResource)
		{
			if (!refMesh.IsVaild())
			{
				continue;
			}
			Mesh* mesh = AssetManager::GetInstance()->LoadResource<Mesh>(refMesh);

			if (mesh == nullptr)
			{
				LOG_ERROR("引用的Mesh资源不存在 {0}", refMesh.Data());
				continue;
			}

			std::vector<SubMesh>& subMeshes = mesh->Meshes;

			int end = Math::Min(subMeshes.size(), refMaterials.size());

			for (int i = 0; i < end; ++i)
			{
				if (!refMaterials[i].IsVaild())
				{
					continue;
				}

				MaterialData* material = AssetManager::GetInstance()->LoadResource<MaterialData>(refMaterials[i]);

				if (material == nullptr)
				{
					LOG_ERROR("引用的Material资源不存在 {0}", refMaterials[i].Data());
					continue;
				}

				cccc[material][&subMeshes[i]].push_back(constant);
			}
		}

		for (const auto& value : cccc)
		{
			const MaterialData* refMaterial = value.first;
			const auto& pairs = value.second;
			for (const auto & vv : pairs)
			{
				const SubMesh* refSubMesh = vv.first;
				const std::vector<ObjectConstants>& c = vv.second;

				MaterialResource material;

				// 加载Shader 创建PSO
				material.Descriptor.mInputLayoutName = "BaseInputLayout";
				material.Descriptor.mRasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
				material.Descriptor.mDepthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
				material.Descriptor.mDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_EQUAL;
				material.Descriptor.mDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
				material.Descriptor.mRTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
				material.Descriptor.mRTVFormats[1] = DXGI_FORMAT_R8G8B8A8_SNORM;
				material.Descriptor.mRTVFormats[2] = DXGI_FORMAT_R32G32B32A32_FLOAT;
				material.Descriptor.mRTVFormats[3] = DXGI_FORMAT_R8G8B8A8_UNORM;
				material.Descriptor.mRTVFormats[4] = DXGI_FORMAT_R16G16_FLOAT;
				material.Descriptor.mNumRenderTargets = 5;
				material.Descriptor.mDepthStencilFormat = mRHI->GetViewportInfo().DepthStencilFormat;
				material.Descriptor.m4xMsaaState = false;

				if (false)
				{
					ASSERT(0);
				}
				else
				{
					material.Descriptor.mShader = mDefaultBasePassShader.get();
				}
				// End
				mGraphicsPSOManager->TryCreatePSO(material.Descriptor);

				// 材质参数
				if (refMaterial->RefDiffuseTexture.IsVaild())
				{
					TextureData* texture = AssetManager::GetInstance()->LoadResource<TextureData>(refMaterial->RefDiffuseTexture);
					if (texture == nullptr)
					{
						LOG_ERROR("Material资源的漫反射纹理贴图没有找到路径 {0}", refMaterial->RefDiffuseTexture.Data());
						continue;
					}

					if (mTextureMap.find(refMaterial->RefDiffuseTexture) == mTextureMap.end())
					{
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
						
						D3D12TextureRef shared_ptr = mRHI->CreateTexture(info, TexCreate_SRV);
						if (shared_ptr == nullptr)
						{
							LOG_ERROR("D3D贴图资源创建失败 {0}", refMaterial->RefDiffuseTexture.Data());
							continue;
						}

						mRHI->UploadTextureData(shared_ptr, texture);

						mTextureMap[refMaterial->RefDiffuseTexture] = shared_ptr;
					}
					material.DiffuseTexture = mTextureMap[refMaterial->RefDiffuseTexture];

				}
				else
				{
					LOG_ERROR("Material资源的漫反射纹理贴图不存在");
					continue;
				}
				material.cbPass = mRenderCameraBuffer;

				// End

				SubMeshResource subMesh;
				
				// 加载模型资源
				if (mVertexMap.find(refSubMesh->mGuid) == mVertexMap.end())
				{
					D3D12VertexBufferRef vertexBuffer = mRHI->CreateVertexBuffer(refSubMesh->Vertices.data(), sizeof(Vertex) * refSubMesh->Vertices.size());
					D3D12IndexBufferRef indexBuffer = mRHI->CreateIndexBuffer(refSubMesh->Indices.data(), sizeof(unsigned) * refSubMesh->Indices.size());

					mVertexMap[refSubMesh->mGuid] = vertexBuffer;
					mIndexMap[refSubMesh->mGuid] = indexBuffer;
				}
				// End


				subMesh.VertexByteStride = sizeof(Vertex);
				subMesh.VertexBufferByteSize = subMesh.VertexByteStride * refSubMesh->Vertices.size();
				subMesh.IndexFormat = DXGI_FORMAT_R32_UINT;
				subMesh.IndexBufferByteSize = sizeof(unsigned) * refSubMesh->Indices.size();
				subMesh.PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

				subMesh.VertexBufferRef = mVertexMap[refSubMesh->mGuid];
				subMesh.IndexBufferRef = mIndexMap[refSubMesh->mGuid];

				subMesh.IndexCount = refSubMesh->Indices.size();
				subMesh.StartIndexLocation = 0;
				subMesh.BaseVertexLocation = 0;

				D3D12StructuredBufferRef structured_buffer_ref = mRHI->CreateStructuredBuffer(c.data(), sizeof(ObjectConstants), c.size());
				mBasePassBatchs.emplace_back(material, subMesh, structured_buffer_ref, c.size());
			}
		}


		// Update Light
		if (mDirectionalLightResource.size() > 0)
		{
			mDirectionalLightStructuredBufferRef = mRHI->CreateStructuredBuffer(mDirectionalLightResource.data(), sizeof(DirectionalLight), mDirectionalLightResource.size());
		}
		else
		{
			mDirectionalLightStructuredBufferRef = nullptr;
		}
		if (mPointLightResource.size() > 0)
		{
			mPointLightStructuredBufferRef = mRHI->CreateStructuredBuffer(mPointLightResource.data(), sizeof(PointLight), mPointLightResource.size());
		}
		else
		{
			mPointLightStructuredBufferRef = nullptr;
		}
		if (mSpotLightResource.size() > 0)
		{
			mSpotLightStructuredBufferRef = mRHI->CreateStructuredBuffer(mSpotLightResource.data(), sizeof(SpotLight), mSpotLightResource.size());
		}
		else
		{
			mSpotLightStructuredBufferRef = nullptr;
		}

		mLightCommonData.DirectionalLightCount = (uint32_t)mDirectionalLightResource.size();
		mLightCommonData.PointLightCount = (uint32_t)mPointLightResource.size();
		mLightCommonData.SpotLightCount = (uint32_t)mSpotLightResource.size();
		mLightCommonDataBuffer = mRHI->CreateConstantBuffer(&mLightCommonData, sizeof(mLightCommonData));
	}

	void D3D12RenderResource::TryCreatePSO(const GraphicsPSODescriptor& descriptor)
	{
		mGraphicsPSOManager->TryCreatePSO(descriptor);
	}

	ID3D12PipelineState* D3D12RenderResource::GetPSO(const GraphicsPSODescriptor& descriptor)
	{
		return mGraphicsPSOManager->GetPSO(descriptor);
	}

	const std::vector<std::tuple<MaterialResource, SubMeshResource, D3D12StructuredBufferRef, uint32_t>>& D3D12RenderResource::GetBasePassBatchs() const
	{
		return mBasePassBatchs;
	}

	D3D12VertexBufferRef D3D12RenderResource::GetMeshVertexBufferRef(const GUID& guid)
	{
		ASSERT(mVertexMap.find(guid) != mVertexMap.end());

		return mVertexMap[guid];
	}

	D3D12IndexBufferRef D3D12RenderResource::GetMeshIndexBufferRef(const GUID& guid)
	{
		ASSERT(mIndexMap.find(guid) != mIndexMap.end());

		return mIndexMap[guid];
	}

	D3D12ConstantBufferRef D3D12RenderResource::GetCbPassRef()
	{
		return mRenderCameraBuffer;
	}

	void D3D12RenderResource::UploadDefaultVertexResource()
	{
		{
			Mesh* sphereMesh = AssetManager::GetInstance()->LoadDefaultMeshResource(DefaultMesh_Sphere);
			ASSERT(sphereMesh != nullptr);
			SubMesh& sub_mesh = sphereMesh->Meshes[0];
			D3D12VertexBufferRef vertexBuffer = mRHI->CreateVertexBuffer(sub_mesh.Vertices.data(), sizeof(Vertex) * sub_mesh.Vertices.size());
			D3D12IndexBufferRef indexBuffer = mRHI->CreateIndexBuffer(sub_mesh.Indices.data(), sizeof(unsigned) * sub_mesh.Indices.size());

			mVertexMap[sub_mesh.mGuid] = vertexBuffer;
			mIndexMap[sub_mesh.mGuid] = indexBuffer;
		}

		{
			Mesh* boxMesh = AssetManager::GetInstance()->LoadDefaultMeshResource(DefaultMesh_Box);
			ASSERT(boxMesh != nullptr);
			SubMesh& sub_mesh = boxMesh->Meshes[0];
			D3D12VertexBufferRef vertexBuffer = mRHI->CreateVertexBuffer(sub_mesh.Vertices.data(), sizeof(Vertex) * sub_mesh.Vertices.size());
			D3D12IndexBufferRef indexBuffer = mRHI->CreateIndexBuffer(sub_mesh.Indices.data(), sizeof(unsigned) * sub_mesh.Indices.size());
			
			mVertexMap[sub_mesh.mGuid] = vertexBuffer;
			mIndexMap[sub_mesh.mGuid] = indexBuffer;
		}

		{
			Mesh* cylinderMesh = AssetManager::GetInstance()->LoadDefaultMeshResource(DefaultMesh_Cylinder);
			ASSERT(cylinderMesh != nullptr);
			SubMesh& sub_mesh = cylinderMesh->Meshes[0];
			D3D12VertexBufferRef vertexBuffer = mRHI->CreateVertexBuffer(sub_mesh.Vertices.data(), sizeof(Vertex) * sub_mesh.Vertices.size());
			D3D12IndexBufferRef indexBuffer = mRHI->CreateIndexBuffer(sub_mesh.Indices.data(), sizeof(unsigned) * sub_mesh.Indices.size());

			mVertexMap[sub_mesh.mGuid] = vertexBuffer;
			mIndexMap[sub_mesh.mGuid] = indexBuffer;
		}

		{
			Mesh* gridMesh = AssetManager::GetInstance()->LoadDefaultMeshResource(DefaultMesh_Grid);
			ASSERT(gridMesh != nullptr);
			SubMesh& sub_mesh = gridMesh->Meshes[0];
			D3D12VertexBufferRef vertexBuffer = mRHI->CreateVertexBuffer(sub_mesh.Vertices.data(), sizeof(Vertex) * sub_mesh.Vertices.size());
			D3D12IndexBufferRef indexBuffer = mRHI->CreateIndexBuffer(sub_mesh.Indices.data(), sizeof(unsigned) * sub_mesh.Indices.size());

			mVertexMap[sub_mesh.mGuid] = vertexBuffer;
			mIndexMap[sub_mesh.mGuid] = indexBuffer;
		}

		{
			Mesh* quadMesh = AssetManager::GetInstance()->LoadDefaultMeshResource(DefaultMesh_Quad);
			ASSERT(quadMesh != nullptr);
			SubMesh& sub_mesh = quadMesh->Meshes[0];
			D3D12VertexBufferRef vertexBuffer = mRHI->CreateVertexBuffer(sub_mesh.Vertices.data(), sizeof(Vertex) * sub_mesh.Vertices.size());
			D3D12IndexBufferRef indexBuffer = mRHI->CreateIndexBuffer(sub_mesh.Indices.data(), sizeof(unsigned) * sub_mesh.Indices.size());

			mVertexMap[sub_mesh.mGuid] = vertexBuffer;
			mIndexMap[sub_mesh.mGuid] = indexBuffer;
		}

		{
			Mesh* screenQuadMesh = AssetManager::GetInstance()->LoadDefaultMeshResource(DefaultMesh_ScreenQuad);
			ASSERT(screenQuadMesh != nullptr);
			SubMesh& sub_mesh = screenQuadMesh->Meshes[0];
			D3D12VertexBufferRef vertexBuffer = mRHI->CreateVertexBuffer(sub_mesh.Vertices.data(), sizeof(Vertex) * sub_mesh.Vertices.size());
			D3D12IndexBufferRef indexBuffer = mRHI->CreateIndexBuffer(sub_mesh.Indices.data(), sizeof(unsigned) * sub_mesh.Indices.size());

			mVertexMap[sub_mesh.mGuid] = vertexBuffer;
			mIndexMap[sub_mesh.mGuid] = indexBuffer;
		}
	}

	void D3D12RenderResource::CreateInputLayout()
	{
		{
			std::vector<D3D12_INPUT_ELEMENT_DESC> baseInputLayout =
			{
				// Vector3 坐标
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				// Vector3 法线
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				// Vector3 切线
				{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				// Vector3 副切线
				{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				// Vector2 纹理坐标
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			};

			mInputLayoutManager.AddInputLayout("BaseInputLayout", baseInputLayout);
		}
	}
}
