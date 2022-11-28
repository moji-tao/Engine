#include <cassert>
#include "EngineRuntime/include/Function/Render/DirectX/D3D12PiplineState.h"
#include "EngineRuntime/include/Function/Render/DirectX/DirectXUtil.h"

namespace Engine
{
	bool GraphicsPSODescriptor::operator==(const GraphicsPSODescriptor& other) const
	{
		return other.mInputLayoutName == mInputLayoutName
			&& other.mShader == mShader
			&& other.mPrimitiveTopologyType == mPrimitiveTopologyType
			&& other.mRasterizerDesc.CullMode == mRasterizerDesc.CullMode
			&& other.mDepthStencilDesc.DepthFunc == mDepthStencilDesc.DepthFunc;
	}

	bool ComputePSODescriptor::operator==(const ComputePSODescriptor& other) const
	{
		return other.mShader == mShader
			&& other.mFlags == mFlags;
	}

	GraphicsPSOManager::GraphicsPSOManager(D3D12RHI* inD3D12RHI, InputLayoutManager* inInputLayoutManager)
		:mRHI(inD3D12RHI), mInputLayoutManager(inInputLayoutManager)
	{ }

	void GraphicsPSOManager::TryCreatePSO(const GraphicsPSODescriptor& descriptor)
	{
		if (mPSOMap.find(descriptor) == mPSOMap.end())
		{
			CreatePSO(descriptor);
		}
	}

	ID3D12PipelineState* GraphicsPSOManager::GetPSO(const GraphicsPSODescriptor& descriptor)
	{
		auto it = mPSOMap.find(descriptor);

		if(it == mPSOMap.end())
		{
			assert(0);

			return nullptr;
		}

		return it->second.Get();
	}

	void GraphicsPSOManager::CreatePSO(const GraphicsPSODescriptor& descriptor)
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
		memset(&psoDesc, 0x00, sizeof(psoDesc));

		std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayouts;
		mInputLayoutManager->GetInputLayout(descriptor.mInputLayoutName, inputLayouts);
		psoDesc.InputLayout.NumElements = inputLayouts.size();
		psoDesc.InputLayout.pInputElementDescs = inputLayouts.data();

		Shader* shader = descriptor.mShader;
		psoDesc.pRootSignature = shader->mRootSignature.Get();
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(shader->mShaderInfo.mVertexShader->GetData(), shader->mShaderInfo.mVertexShader->GetSize());
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(shader->mShaderInfo.mPixelShader->GetData(), shader->mShaderInfo.mPixelShader->GetSize());

		psoDesc.RasterizerState = descriptor.mRasterizerDesc;
		psoDesc.BlendState = descriptor.mBlendDesc;
		psoDesc.DepthStencilState = descriptor.mDepthStencilDesc;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = descriptor.mPrimitiveTopologyType;
		psoDesc.NumRenderTargets = descriptor.mNumRenderTargets;
		for (int i = 0; i < 8; ++i)
		{
			psoDesc.RTVFormats[i] = descriptor.mRTVFormats[i];
		}
		psoDesc.SampleDesc.Count = descriptor.m4xMsaaState ? 4 : 1;
		psoDesc.SampleDesc.Quality = descriptor.m4xMsaaState ? (descriptor.m4xMsaaQuality - 1) : 0;
		psoDesc.DSVFormat = descriptor.mDepthStencilFormat;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> PSO;
		auto device = mRHI->GetDevice()->GetNativeDevice();
		ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(PSO.ReleaseAndGetAddressOf())), "创建PSO失败");

		mPSOMap.emplace(descriptor, PSO);
	}

	ComputePSOManager::ComputePSOManager(D3D12RHI* inD3D12RHI)
		:mRHI(inD3D12RHI)
	{ }

	void ComputePSOManager::TryCreatePSO(const ComputePSODescriptor& descriptor)
	{
		if (mPSOMap.find(descriptor) == mPSOMap.end())
		{
			CreatePSO(descriptor);
		}
	}

	ID3D12PipelineState* ComputePSOManager::GetPSO(const ComputePSODescriptor& descriptor)
	{
		auto it = mPSOMap.find(descriptor);

		if (it == mPSOMap.end())
		{
			assert(0);

			return nullptr;
		}

		return it->second.Get();
	}

	void ComputePSOManager::CreatePSO(const ComputePSODescriptor& descriptor)
	{
		D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
		Shader* shader = descriptor.mShader;
		psoDesc.pRootSignature = shader->mRootSignature.Get();
		psoDesc.CS = CD3DX12_SHADER_BYTECODE(shader->mShaderInfo.mComputeShader->GetData(), shader->mShaderInfo.mComputeShader->GetSize());
		psoDesc.Flags = descriptor.mFlags;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> PSO;
		auto device = mRHI->GetDevice()->GetNativeDevice();
		ThrowIfFailed(device->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(PSO.ReleaseAndGetAddressOf())), "创建PSO失败");

		mPSOMap.emplace(descriptor, PSO);
	}
}
