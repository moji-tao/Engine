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
		mComputePSOManager = std::make_unique<ComputePSOManager>(mRHI);

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

		mNullSRV = mRHI->CreateStructuredBuffer("123", 3, 1);

		SSAOCBParameter ssaoParameter;
		ssaoParameter.OcclusionRadius = 0.03f;
		ssaoParameter.OcclusionFadeStart = 0.01f;
		ssaoParameter.OcclusionFadeEnd = 0.03f;
		ssaoParameter.SurfaceEpsilon = 0.001f;
		mRenderSSAOCBBuffer = mRHI->CreateConstantBuffer(&ssaoParameter, sizeof(ssaoParameter));

		CreateNullTexture();

		CreateInputLayout();
	}

	D3D12RenderResource::~D3D12RenderResource()
	{
		mGraphicsPSOManager = nullptr;
		mComputePSOManager = nullptr;
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

		for (const auto& value : mCameraRenderNoSkeletonResource)
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
				material.Descriptor.mRTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;	// Position
				material.Descriptor.mRTVFormats[1] = DXGI_FORMAT_R8G8B8A8_SNORM;		// Normal
				material.Descriptor.mRTVFormats[2] = DXGI_FORMAT_R32G32B32A32_FLOAT;	// BaseColor
				material.Descriptor.mRTVFormats[3] = DXGI_FORMAT_R8G8B8A8_UNORM;		// MetallicRoughness
				material.Descriptor.mRTVFormats[4] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Emissive
				material.Descriptor.mRTVFormats[5] = DXGI_FORMAT_R16G16_FLOAT;			// Velocity
				material.Descriptor.mNumRenderTargets = 6;
				material.Descriptor.mDepthStencilFormat = mRHI->GetViewportInfo().DepthStencilFormat;
				material.Descriptor.m4xMsaaState = false;

				material.Descriptor.mShader = mDefaultBasePassShader.get();
				// End
				mGraphicsPSOManager->TryCreatePSO(material.Descriptor);

				// 材质参数
				ResourceMaterialData materialData;

				// 绑定漫反射纹理
				do
				{
					materialData.AlbedoColor = refMaterial->Albedo;

					if (refMaterial->RefAlbedoTexture.IsVaild())
					{
						TextureData* texture = AssetManager::GetInstance()->LoadResource<TextureData>(refMaterial->RefAlbedoTexture);
						
						if (texture == nullptr)
						{
							materialData.HasAlbedoTexture = 0;
							material.DiffuseTexture = mNullTextureResource;
							break;
						}

						if (mTextureMap.find(refMaterial->RefAlbedoTexture) == mTextureMap.end())
						{
							if (!UploadTexture(refMaterial->RefAlbedoTexture, texture))
							{
								materialData.HasAlbedoTexture = 0;
								material.DiffuseTexture = mNullTextureResource;
								break;
							}
						}
						materialData.HasAlbedoTexture = 1;
						material.DiffuseTexture = mTextureMap[refMaterial->RefAlbedoTexture];

					}
					else
					{
						materialData.HasAlbedoTexture = 0;
						material.DiffuseTexture = mNullTextureResource;
						break;
					}

				} while (false);

				// 绑定法线纹理
				do
				{
					if (refMaterial->RefNormalTexture.IsVaild())
					{
						TextureData* texture = AssetManager::GetInstance()->LoadResource<TextureData>(refMaterial->RefNormalTexture);
						if (texture == nullptr)
						{
							materialData.HasNormalTexture = 0;
							material.NormalTexture = mNullTextureResource;
							break;
						}

						if (mTextureMap.find(refMaterial->RefNormalTexture) == mTextureMap.end())
						{
							if (!UploadTexture(refMaterial->RefNormalTexture, texture))
							{
								materialData.HasNormalTexture = 0;
								material.NormalTexture = mNullTextureResource;
								break;
							}
						}
						materialData.HasNormalTexture = 1;
						material.NormalTexture = mTextureMap[refMaterial->RefNormalTexture];

					}
					else
					{
						materialData.HasNormalTexture = 0;
						material.NormalTexture = mNullTextureResource;
						break;
					}
				} while (false);

				// 绑定金属纹理
				do
				{
					if (refMaterial->RefMetallicTexture.IsVaild())
					{
						TextureData* texture = AssetManager::GetInstance()->LoadResource<TextureData>(refMaterial->RefMetallicTexture);
						if (texture == nullptr)
						{
							materialData.HasMetallicTexture = 0;
							material.MetallicTexture = mNullTextureResource;
							break;
						}

						if (mTextureMap.find(refMaterial->RefMetallicTexture) == mTextureMap.end())
						{
							if (!UploadTexture(refMaterial->RefMetallicTexture, texture))
							{
								materialData.HasMetallicTexture = 0;
								material.MetallicTexture = mNullTextureResource;
								break;
							}
						}
						materialData.HasMetallicTexture = 1;
						material.MetallicTexture = mTextureMap[refMaterial->RefMetallicTexture];

					}
					else
					{
						materialData.HasMetallicTexture = 0;
						material.MetallicTexture = mNullTextureResource;
						break;
					}
				} while (false);

				// 绑定粗糙度纹理
				do
				{
					if (refMaterial->RefRoughnessTexture.IsVaild())
					{
						TextureData* texture = AssetManager::GetInstance()->LoadResource<TextureData>(refMaterial->RefRoughnessTexture);
						if (texture == nullptr)
						{
							materialData.HasRoughnessTexture = 0;
							material.RoughnessTexture = mNullTextureResource;
							break;
						}

						if (mTextureMap.find(refMaterial->RefRoughnessTexture) == mTextureMap.end())
						{
							if (!UploadTexture(refMaterial->RefRoughnessTexture, texture))
							{
								materialData.HasRoughnessTexture = 0;
								material.RoughnessTexture = mNullTextureResource;
								break;
							}
						}
						materialData.HasRoughnessTexture = 1;
						material.RoughnessTexture = mTextureMap[refMaterial->RefRoughnessTexture];

					}
					else
					{
						materialData.HasRoughnessTexture = 0;
						material.RoughnessTexture = mNullTextureResource;
						break;
					}
				} while (false);

				materialData.Emissive = refMaterial->Emissive;

				material.cbPass = mRenderCameraBuffer;

				material.cbMaterialData = mRHI->CreateConstantBuffer(&materialData, sizeof(materialData));

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

				ModleResource modleResource;
				modleResource.gInstanceDataD3D12StructuredBufferRef = mRHI->CreateStructuredBuffer(c.data(), sizeof(ObjectConstants), c.size());
				modleResource.gBoneTransformsD3D12StructuredBufferRef = mNullSRV;

				mBasePassBatchs.emplace_back(material, subMesh, modleResource, c.size());
			}
		}

		for (const auto& value : mCameraRenderSkeletonResource)
		{
			const MaterialData* refMaterial = value.first;
			const auto& pairs = value.second;
			for (const auto& vv : pairs)
			{
				const SubMesh* refSubMesh = vv.first;
				std::vector<std::pair<ObjectConstants, std::vector<Matrix4x4>>> c = vv.second;

				MaterialResource material;

				// 加载Shader 创建PSO
				material.Descriptor.mInputLayoutName = "BaseInputLayout";
				material.Descriptor.mRasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
				material.Descriptor.mDepthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
				material.Descriptor.mDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_EQUAL;
				material.Descriptor.mDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
				material.Descriptor.mRTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;	// Position
				material.Descriptor.mRTVFormats[1] = DXGI_FORMAT_R8G8B8A8_SNORM;		// Normal
				material.Descriptor.mRTVFormats[2] = DXGI_FORMAT_R32G32B32A32_FLOAT;	// BaseColor
				material.Descriptor.mRTVFormats[3] = DXGI_FORMAT_R8G8B8A8_UNORM;		// MetallicRoughness
				material.Descriptor.mRTVFormats[4] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Emissive
				material.Descriptor.mRTVFormats[5] = DXGI_FORMAT_R16G16_FLOAT;			// Velocity
				material.Descriptor.mNumRenderTargets = 6;
				material.Descriptor.mDepthStencilFormat = mRHI->GetViewportInfo().DepthStencilFormat;
				material.Descriptor.m4xMsaaState = false;

				material.Descriptor.mShader = mDefaultBasePassShader.get();
				// End
				mGraphicsPSOManager->TryCreatePSO(material.Descriptor);

				// 材质参数
				ResourceMaterialData materialData;

				// 绑定漫反射纹理
				do
				{
					materialData.AlbedoColor = refMaterial->Albedo;

					if (refMaterial->RefAlbedoTexture.IsVaild())
					{
						TextureData* texture = AssetManager::GetInstance()->LoadResource<TextureData>(refMaterial->RefAlbedoTexture);

						if (texture == nullptr)
						{
							materialData.HasAlbedoTexture = 0;
							material.DiffuseTexture = mNullTextureResource;
							break;
						}

						if (mTextureMap.find(refMaterial->RefAlbedoTexture) == mTextureMap.end())
						{
							if (!UploadTexture(refMaterial->RefAlbedoTexture, texture))
							{
								materialData.HasAlbedoTexture = 0;
								material.DiffuseTexture = mNullTextureResource;
								break;
							}
						}
						materialData.HasAlbedoTexture = 1;
						material.DiffuseTexture = mTextureMap[refMaterial->RefAlbedoTexture];

					}
					else
					{
						materialData.HasAlbedoTexture = 0;
						material.DiffuseTexture = mNullTextureResource;
						break;
					}

				} while (false);

				// 绑定法线纹理
				do
				{
					if (refMaterial->RefNormalTexture.IsVaild())
					{
						TextureData* texture = AssetManager::GetInstance()->LoadResource<TextureData>(refMaterial->RefNormalTexture);
						if (texture == nullptr)
						{
							materialData.HasNormalTexture = 0;
							material.NormalTexture = mNullTextureResource;
							break;
						}

						if (mTextureMap.find(refMaterial->RefNormalTexture) == mTextureMap.end())
						{
							if (!UploadTexture(refMaterial->RefNormalTexture, texture))
							{
								materialData.HasNormalTexture = 0;
								material.NormalTexture = mNullTextureResource;
								break;
							}
						}
						materialData.HasNormalTexture = 1;
						material.NormalTexture = mTextureMap[refMaterial->RefNormalTexture];

					}
					else
					{
						materialData.HasNormalTexture = 0;
						material.NormalTexture = mNullTextureResource;
						break;
					}
				} while (false);

				// 绑定金属纹理
				do
				{
					if (refMaterial->RefMetallicTexture.IsVaild())
					{
						TextureData* texture = AssetManager::GetInstance()->LoadResource<TextureData>(refMaterial->RefMetallicTexture);
						if (texture == nullptr)
						{
							materialData.HasMetallicTexture = 0;
							material.MetallicTexture = mNullTextureResource;
							break;
						}

						if (mTextureMap.find(refMaterial->RefMetallicTexture) == mTextureMap.end())
						{
							if (!UploadTexture(refMaterial->RefMetallicTexture, texture))
							{
								materialData.HasMetallicTexture = 0;
								material.MetallicTexture = mNullTextureResource;
								break;
							}
						}
						materialData.HasMetallicTexture = 1;
						material.MetallicTexture = mTextureMap[refMaterial->RefMetallicTexture];

					}
					else
					{
						materialData.HasMetallicTexture = 0;
						material.MetallicTexture = mNullTextureResource;
						break;
					}
				} while (false);

				// 绑定粗糙度纹理
				do
				{
					if (refMaterial->RefRoughnessTexture.IsVaild())
					{
						TextureData* texture = AssetManager::GetInstance()->LoadResource<TextureData>(refMaterial->RefRoughnessTexture);
						if (texture == nullptr)
						{
							materialData.HasRoughnessTexture = 0;
							material.RoughnessTexture = mNullTextureResource;
							break;
						}

						if (mTextureMap.find(refMaterial->RefRoughnessTexture) == mTextureMap.end())
						{
							if (!UploadTexture(refMaterial->RefRoughnessTexture, texture))
							{
								materialData.HasRoughnessTexture = 0;
								material.RoughnessTexture = mNullTextureResource;
								break;
							}
						}
						materialData.HasRoughnessTexture = 1;
						material.RoughnessTexture = mTextureMap[refMaterial->RefRoughnessTexture];

					}
					else
					{
						materialData.HasRoughnessTexture = 0;
						material.RoughnessTexture = mNullTextureResource;
						break;
					}
				} while (false);

				materialData.Emissive = refMaterial->Emissive;

				material.cbPass = mRenderCameraBuffer;

				material.cbMaterialData = mRHI->CreateConstantBuffer(&materialData, sizeof(materialData));

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

				for (size_t i = 0; i < c.size(); i++)
				{
					ModleResource modleResource;
					modleResource.gInstanceDataD3D12StructuredBufferRef = mRHI->CreateStructuredBuffer(&c[i].first, sizeof(ObjectConstants), 1);
					//modleResource.gBoneTransformsD3D12StructuredBufferRef = mRHI->CreateStructuredBuffer(c[i].second.data(), sizeof(Matrix4x4) * c[i].second.size(), 1);
					modleResource.gBoneTransformsD3D12StructuredBufferRef = mRHI->CreateStructuredBuffer(c[i].second.data(), sizeof(Matrix4x4), c[i].second.size());
					mBasePassBatchs.emplace_back(material, subMesh, modleResource, 1);
				}
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
		mLightCommonData.EnableSSAO = RenderSystem::GetInstance()->mEnableSSAO;
		mLightCommonData.EnableAmbientLighting = RenderSystem::GetInstance()->mEnableAmbientLighting;
		mLightCommonDataBuffer = mRHI->CreateConstantBuffer(&mLightCommonData, sizeof(mLightCommonData));
	}

	void D3D12RenderResource::TryCreatePSO(const GraphicsPSODescriptor& descriptor)
	{
		mGraphicsPSOManager->TryCreatePSO(descriptor);
	}

	void D3D12RenderResource::TryCreatePSO(const ComputePSODescriptor& descriptor)
	{
		mComputePSOManager->TryCreatePSO(descriptor);
	}

	ID3D12PipelineState* D3D12RenderResource::GetPSO(const GraphicsPSODescriptor& descriptor)
	{
		return mGraphicsPSOManager->GetPSO(descriptor);
	}

	ID3D12PipelineState* D3D12RenderResource::GetPSO(const ComputePSODescriptor& descriptor)
	{
		return mComputePSOManager->GetPSO(descriptor);
	}

	const std::vector<std::tuple<MaterialResource, SubMeshResource, ModleResource, uint32_t>>& D3D12RenderResource::GetBasePassBatchs() const
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

	D3D12ConstantBufferRef D3D12RenderResource::GetSSAOCBRef()
	{
		return mRenderSSAOCBBuffer;
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

	bool D3D12RenderResource::UploadTexture(const GUID& textureGuid, TextureData* texture)
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
			LOG_ERROR("D3D贴图资源创建失败 {0}", textureGuid.Data());
			return false;
		}

		mRHI->UploadTextureData(shared_ptr, texture);

		mTextureMap[textureGuid] = shared_ptr;
		return true;
	}

	void D3D12RenderResource::CreateNullTexture()
	{
		mNullTexture = LoadTextureForFile(EngineFileSystem::GetInstance()->GetActualPath("Resource/Texture/nullT.png"), false);

		D3D12TextureInfo info;
		info.Type = mNullTexture->Info.mType;
		info.Format = D3D12Texture::TransformationToD3DFormat(mNullTexture->Info.mFormat);
		info.Width = mNullTexture->Info.mWidth;
		info.Height = mNullTexture->Info.mHeight;
		// TODO:纹理类型记得扩展
		info.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		info.Depth = mNullTexture->Info.mDepth;
		info.ArraySize = mNullTexture->Info.mArrayLayers;
		info.MipCount = mNullTexture->Info.mMipLevels;

		mNullTextureResource = mRHI->CreateTexture(info, TexCreate_SRV);

		mRHI->UploadTextureData(mNullTextureResource, mNullTexture.get());
	}

	void D3D12RenderResource::CreateInputLayout()
	{
		/*
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
		*/
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
				// Vector4 骨骼索引
				{ "BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 56, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				// Vector4 骨骼权重
				{ "BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 72, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				// uint 骨骼数
				{ "BONENUM", 0, DXGI_FORMAT_R32_UINT, 0, 88, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			};

			mInputLayoutManager.AddInputLayout("BaseInputLayout", baseInputLayout);
		}

	}

	void MaterialResource::BindShaderBindParameters(const ModleResource& modleResource) const
	{

		Descriptor.mShader->SetParameter("gInstanceData", modleResource.gInstanceDataD3D12StructuredBufferRef->GetSRV());
		Descriptor.mShader->SetParameter("gBoneTransforms", modleResource.gBoneTransformsD3D12StructuredBufferRef->GetSRV());
		Descriptor.mShader->SetParameter("cbPass", cbPass);
		Descriptor.mShader->SetParameter("cbMaterialData", cbMaterialData);
		Descriptor.mShader->SetParameter("BaseColorTexture", DiffuseTexture->GetSRV());
		Descriptor.mShader->SetParameter("NormalTexture", NormalTexture->GetSRV());
		Descriptor.mShader->SetParameter("MetallicTexture", MetallicTexture->GetSRV());
		Descriptor.mShader->SetParameter("RoughnessTexture", RoughnessTexture->GetSRV());
		Descriptor.mShader->BindParameters();

	}
}
