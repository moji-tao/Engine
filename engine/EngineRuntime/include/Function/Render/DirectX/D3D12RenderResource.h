#pragma once
#include <unordered_map>
#include "EngineRuntime/include/Function/Render/RenderResource.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12Buffer.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12RHI.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12InputLayout.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12PiplineState.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/MaterialData.h"

namespace Engine
{
	struct SubMeshResource
	{
		uint32_t VertexByteStride;
		uint32_t VertexBufferByteSize;
		DXGI_FORMAT IndexFormat;
		uint32_t IndexBufferByteSize;
		D3D12_PRIMITIVE_TOPOLOGY PrimitiveType;

		D3D12VertexBufferRef VertexBufferRef;
		D3D12IndexBufferRef IndexBufferRef;

		uint32_t IndexCount;
		uint32_t StartIndexLocation;
		uint32_t BaseVertexLocation;
	};

	struct ResourceMaterialData
	{
		uint32_t HasAlbedoTexture = 0;
		uint32_t HasNormalTexture = 0;
		uint32_t HasMetallicTexture = 0;
		uint32_t HasRoughnessTexture = 0;

		Vector4 AlbedoColor;
		Vector3 Emissive;
		float Metallic;
		float Roughness;
	};

	struct SSAOCBParameter
	{
		float OcclusionRadius;
		float SurfaceEpsilon;
		float OcclusionFadeEnd;
		float OcclusionFadeStart;
	};

	struct MaterialResource
	{
		GraphicsPSODescriptor Descriptor;

		void BindShaderBindParameters(D3D12StructuredBufferRef cbRef) const;

		D3D12TextureRef DiffuseTexture;
		D3D12TextureRef NormalTexture;
		D3D12TextureRef MetallicTexture;
		D3D12TextureRef RoughnessTexture;

		D3D12ConstantBufferRef cbPass;

		D3D12ConstantBufferRef cbMaterialData;
	};

	class D3D12RenderResource : public RenderResource
	{
	public:
		D3D12RenderResource(D3D12RHI* rhi);

		virtual ~D3D12RenderResource() override;

		virtual void EndFrameBuffer() override;

	public:
		void UpdateRenderResource();

		void TryCreatePSO(const GraphicsPSODescriptor& descriptor);
		void TryCreatePSO(const ComputePSODescriptor& descriptor);

		ID3D12PipelineState* GetPSO(const GraphicsPSODescriptor& descriptor);
		ID3D12PipelineState* GetPSO(const ComputePSODescriptor& descriptor);

		const std::vector<std::tuple<MaterialResource, SubMeshResource, D3D12StructuredBufferRef, uint32_t>>& GetBasePassBatchs() const;

		D3D12VertexBufferRef GetMeshVertexBufferRef(const GUID& guid);

		D3D12IndexBufferRef GetMeshIndexBufferRef(const GUID& guid);

		D3D12ConstantBufferRef GetCbPassRef();

		D3D12ConstantBufferRef GetSSAOCBRef();

		void UploadDefaultVertexResource();

	private:
		bool UploadTexture(const GUID& textureGuid, TextureData* texture);

		void CreateNullTexture();

	private:
		void CreateInputLayout();

	private:
		D3D12RHI* mRHI = nullptr;

	private:
		std::unordered_map<GUID, D3D12ConstantBufferRef> mConstantMap;
		std::unordered_map<GUID, D3D12StructuredBufferRef> mStructuredMap;
		std::unordered_map<GUID, D3D12RWStructuredBufferRef> mRWStructuredMap;
		std::unordered_map<GUID, D3D12VertexBufferRef> mVertexMap;
		std::unordered_map<GUID, D3D12IndexBufferRef> mIndexMap;
		std::unordered_map<GUID, D3D12ReadBackBufferRef> mReadBackMap;

		std::unordered_map<GUID, D3D12TextureRef> mTextureMap;

	private:
		//GameObject
		std::vector<std::tuple<MaterialResource, SubMeshResource, D3D12StructuredBufferRef, uint32_t>> mBasePassBatchs;

		//Camera
		D3D12ConstantBufferRef mRenderCameraBuffer;

		//SSAO
		D3D12ConstantBufferRef mRenderSSAOCBBuffer;

	public:
		//Light
		D3D12ConstantBufferRef mLightCommonDataBuffer;
		D3D12StructuredBufferRef mDirectionalLightStructuredBufferRef;
		D3D12StructuredBufferRef mPointLightStructuredBufferRef;
		D3D12StructuredBufferRef mSpotLightStructuredBufferRef;

	public:
		D3D12StructuredBufferRef mNullSRV;

		D3D12TextureRef mNullTextureResource;

		std::shared_ptr<TextureData> mNullTexture;

	private:
		std::unique_ptr<Shader> mDefaultBasePassShader;

		InputLayoutManager mInputLayoutManager;

		std::unique_ptr<GraphicsPSOManager> mGraphicsPSOManager;
		std::unique_ptr<ComputePSOManager> mComputePSOManager;
	};
}
