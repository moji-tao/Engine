#pragma once
#include <unordered_map>
#include "EngineRuntime/include/Function/Render/RenderResource.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12Buffer.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12RHI.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12InputLayout.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12PiplineState.h"

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

	struct MaterialResource
	{
		GraphicsPSODescriptor Descriptor;

		void BindShaderBindParameters(D3D12StructuredBufferRef cbRef) const
		{
			Descriptor.mShader->SetParameter("gInstanceData", cbRef->GetSRV());
			Descriptor.mShader->SetParameter("cbPass", cbPass);
			Descriptor.mShader->SetParameter("BaseColorTexture", DiffuseTexture->GetSRV());
			//Descriptor.mShader->SetParameter()
			Descriptor.mShader->BindParameters();
		}

		D3D12TextureRef DiffuseTexture;

		D3D12ConstantBufferRef cbPass;
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

		ID3D12PipelineState* GetPSO(const GraphicsPSODescriptor& descriptor);

		const std::vector<std::tuple<MaterialResource, SubMeshResource, D3D12StructuredBufferRef, uint32_t>>& GetBasePassBatchs() const;

		D3D12VertexBufferRef GetMeshVertexBufferRef(const GUID& guid);

		D3D12IndexBufferRef GetMeshIndexBufferRef(const GUID& guid);

		D3D12ConstantBufferRef GetCbPassRef();

		void UploadDefaultVertexResource();

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

	public:
		//Light
		D3D12ConstantBufferRef mLightCommonDataBuffer;
		D3D12StructuredBufferRef mDirectionalLightStructuredBufferRef;
		D3D12StructuredBufferRef mPointLightStructuredBufferRef;
		D3D12StructuredBufferRef mSpotLightStructuredBufferRef;

	private:
		std::unique_ptr<Shader> mDefaultBasePassShader;

		InputLayoutManager mInputLayoutManager;

		std::unique_ptr<GraphicsPSOManager> mGraphicsPSOManager;
	};
}
