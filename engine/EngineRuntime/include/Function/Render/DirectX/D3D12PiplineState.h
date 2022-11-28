#pragma once
#include <string>
#include <unordered_map>
#include "EngineRuntime/include/Function/Render/DirectX/D3D12Shader.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12InputLayout.h"

namespace Engine
{
	struct GraphicsPSODescriptor
	{
		std::string mInputLayoutName;
		Shader* mShader = nullptr;
		DXGI_FORMAT mRTVFormats[8] = { DXGI_FORMAT_R8G8B8A8_UNORM };
		bool m4xMsaaState = false;
		unsigned m4xMsaaQuality = 0;
		DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		D3D12_RASTERIZER_DESC mRasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		D3D12_BLEND_DESC mBlendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		D3D12_DEPTH_STENCIL_DESC mDepthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		D3D12_PRIMITIVE_TOPOLOGY_TYPE mPrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		unsigned mNumRenderTargets = 1;

	public:
		bool operator==(const GraphicsPSODescriptor& other) const;
	};

	struct ComputePSODescriptor
	{
		Shader* mShader = nullptr;
		D3D12_PIPELINE_STATE_FLAGS mFlags = D3D12_PIPELINE_STATE_FLAG_NONE;

	public:
		bool operator==(const ComputePSODescriptor&) const;
	};
}

template<>
struct std::hash<Engine::GraphicsPSODescriptor>
{
	std::size_t operator()(const Engine::GraphicsPSODescriptor& descriptor) const
	{
		return (std::hash<std::string>()(descriptor.mInputLayoutName) ^ (std::hash<void*>()(descriptor.mShader) << 1));
	}
};

template<>
struct std::hash<Engine::ComputePSODescriptor>
{
	std::size_t operator()(const Engine::ComputePSODescriptor& descriptor) const
	{
		return (std::hash<void*>()(descriptor.mShader) ^ (std::hash<int>()(descriptor.mFlags) << 1));
	}
};

namespace Engine
{
	class GraphicsPSOManager
	{
	public:
		GraphicsPSOManager(D3D12RHI* inD3D12RHI, InputLayoutManager* inInputLayoutManager);

		void TryCreatePSO(const GraphicsPSODescriptor& descriptor);

		ID3D12PipelineState* GetPSO(const GraphicsPSODescriptor& descriptor);

	private:
		void CreatePSO(const GraphicsPSODescriptor& descriptor);

	private:
		D3D12RHI* mRHI = nullptr;

		InputLayoutManager* mInputLayoutManager = nullptr;

		std::unordered_map<GraphicsPSODescriptor, Microsoft::WRL::ComPtr<ID3D12PipelineState>> mPSOMap;
	};

	class ComputePSOManager
	{
	public:
		ComputePSOManager(D3D12RHI* inD3D12RHI);

		void TryCreatePSO(const ComputePSODescriptor& descriptor);

		ID3D12PipelineState* GetPSO(const ComputePSODescriptor& descriptor);

	private:
		void CreatePSO(const ComputePSODescriptor& descriptor);

	private:
		D3D12RHI* mRHI = nullptr;

		std::unordered_map<ComputePSODescriptor, Microsoft::WRL::ComPtr<ID3D12PipelineState>> mPSOMap;
	};

}
