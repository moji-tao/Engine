#include <d3d12shader.h>
#include <d3dcompiler.h>
#include "EngineRuntime/include/Function/Render/DirectX/D3D12Shader.h"
#include "EngineRuntime/include/Function/Render/DirectX/DirectXUtil.h"

namespace Engine
{
	Shader::Shader(const ShaderInfo& InShaderInfo, D3D12RHI* InD3D12RHI)
		:mShaderInfo(InShaderInfo), mD3D12RHI(InD3D12RHI)
	{
		Initialize();

		ASSERT((mShaderInfo.mVertexShader != nullptr || mShaderInfo.mPixelShader != nullptr) ^ mShaderInfo.mComputeShader != nullptr);
	}

	bool Shader::SetParameter(const std::string& ParamName, D3D12ConstantBufferRef ConstantBufferRef)
	{
		bool result = false;

		for (ShaderCBVParameter& param : mCBVParams)
		{
			if (param.mName == ParamName)
			{
				param.mConstantBufferRef = ConstantBufferRef;

				result = true;
			}
		}

		return result;
	}

	bool Shader::SetParameter(const std::string& ParamName, D3D12ShaderResourceView* SRV)
	{
		std::vector<D3D12ShaderResourceView*> SRVList(1);
		SRVList[0] = SRV;

		return SetParameter(ParamName, SRVList);
	}

	bool Shader::SetParameter(const std::string& ParamName, const std::vector<D3D12ShaderResourceView*>& SRVList)
	{
		bool result = false;

		for (ShaderSRVParameter& param : mSRVParams)
		{
			if(param.mName == ParamName)
			{
				ASSERT(SRVList.size() == param.mBindCount);

				param.mSRVList = SRVList;

				result = true;
			}
		}

		return result;
	}

	bool Shader::SetParameter(const std::string& ParamName, D3D12UnorderedAccessView* UAV)
	{
		std::vector<D3D12UnorderedAccessView*> UAVList(1);
		UAVList[0] = UAV;

		return SetParameter(ParamName, UAVList);
	}

	bool Shader::SetParameter(const std::string& ParamName, const std::vector<D3D12UnorderedAccessView*>& UAVList)
	{
		bool result = false;

		for (ShaderUAVParameter& param : mUAVParams)
		{
			if (param.mName == ParamName)
			{
				ASSERT(UAVList.size() == param.mBindCount);

				param.mUAVList = UAVList;

				result = true;
			}
		}

		return result;
	}

	void Shader::BindParameters()
	{
		auto CommandList = mD3D12RHI->GetDevice()->GetCommandList();
		auto DescriptorCache = mD3D12RHI->GetDevice()->GetCommandContext()->GetDescriptorCache();

		CheckBindings();

		bool bComputeShader = mShaderInfo.mComputeShader != nullptr;
		
		// CBV binding
		for (int i = 0; i < mCBVParams.size(); i++)
		{
			UINT RootParamIdx = mCBVSignatureBaseBindSlot + i;
			D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress = mCBVParams[i].mConstantBufferRef->ResourceLocation.GPUVirtualAddress;

			if (bComputeShader)
			{
				CommandList->SetComputeRootConstantBufferView(RootParamIdx, GPUVirtualAddress);
			}
			else
			{
				CommandList->SetGraphicsRootConstantBufferView(RootParamIdx, GPUVirtualAddress);
			}
		}


		// SRV binding
		if (mSRVCount > 0)
		{
			std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> SrcDescriptors;
			SrcDescriptors.resize(mSRVCount);

			for (const ShaderSRVParameter& Param : mSRVParams)
			{
				for (UINT i = 0; i < Param.mSRVList.size(); i++)
				{
					UINT Index = Param.mBindPoint + i;
					SrcDescriptors[Index] = Param.mSRVList[i]->GetDescriptorHandle();
				}
			}

			UINT RootParamIdx = mSRVSignatureBaseBindSlot;
			auto GpuDescriptorHandle = DescriptorCache->AppendCbvSrvUavDescriptors(SrcDescriptors);

			if (bComputeShader)
			{
				CommandList->SetComputeRootDescriptorTable(RootParamIdx, GpuDescriptorHandle);
			}
			else
			{
				CommandList->SetGraphicsRootDescriptorTable(RootParamIdx, GpuDescriptorHandle);
			}
		}

		// UAV binding
		if (mUAVCount > 0)
		{
			std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> SrcDescriptors;
			SrcDescriptors.resize(mUAVCount);

			for (const ShaderUAVParameter& Param : mUAVParams)
			{
				for (UINT i = 0; i < Param.mUAVList.size(); i++)
				{
					UINT Index = Param.mBindPoint + i;
					SrcDescriptors[Index] = Param.mUAVList[i]->GetDescriptorHandle();
				}
			}

			UINT RootParamIdx = mUAVSignatureBaseBindSlot;
			auto GpuDescriptorHandle = DescriptorCache->AppendCbvSrvUavDescriptors(SrcDescriptors);

			if (bComputeShader)
			{
				CommandList->SetComputeRootDescriptorTable(RootParamIdx, GpuDescriptorHandle);
			}
			else
			{
				assert(0);
			}
		}

		ClearBindings();
	}

	void Shader::Initialize()
	{
		if(mShaderInfo.mVertexShader != nullptr)
		{
			GetShaderParameters(mShaderInfo.mVertexShader, ShaderType::VERTEX_SHADER);
		}

		if(mShaderInfo.mPixelShader != nullptr)
		{
			GetShaderParameters(mShaderInfo.mPixelShader, ShaderType::PIXEL_SHADER);
		}

		if(mShaderInfo.mComputeShader != nullptr)
		{
			GetShaderParameters(mShaderInfo.mComputeShader, ShaderType::COMPUTE_SHADER);
		}

		CreateRootSignature();
	}

	void Shader::GetShaderParameters(std::shared_ptr<Blob> ShaderBlob, ShaderType Type)
	{
		ID3D12ShaderReflection* reflection = nullptr;
		D3DReflect(ShaderBlob->GetData(), ShaderBlob->GetSize(), IID_ID3D12ShaderReflection, (void**)&reflection);

		ASSERT(reflection != nullptr);

		D3D12_SHADER_DESC shaderDesc;
		reflection->GetDesc(&shaderDesc);

		for(unsigned i = 0; i < shaderDesc.BoundResources; ++i)
		{
			D3D12_SHADER_INPUT_BIND_DESC resourceDesc;
			reflection->GetResourceBindingDesc(i, &resourceDesc);

			auto shaderVarName = resourceDesc.Name;
			auto resourceType = resourceDesc.Type;
			auto registerSpace = resourceDesc.Space;
			auto bindPoint = resourceDesc.BindPoint;
			auto bindCount = resourceDesc.BindCount;

			if (resourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_CBUFFER)
			{
				ShaderCBVParameter Param;
				Param.mName = shaderVarName;
				Param.mShaderType = Type;
				Param.mBindPoint = bindPoint;
				Param.mRegisterSpace = registerSpace;

				mCBVParams.push_back(Param);
			}
			else if (resourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_STRUCTURED
				|| resourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE)
			{
				ShaderSRVParameter Param;
				Param.mName = shaderVarName;
				Param.mShaderType = Type;
				Param.mBindPoint = bindPoint;
				Param.mBindCount = bindCount;
				Param.mRegisterSpace = registerSpace;

				mSRVParams.push_back(Param);
			}
			else if (resourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWSTRUCTURED
				|| resourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWTYPED)
			{
				ASSERT(Type == ShaderType::COMPUTE_SHADER);

				ShaderUAVParameter Param;
				Param.mName = shaderVarName;
				Param.mShaderType = Type;
				Param.mBindPoint = bindPoint;
				Param.mBindCount = bindCount;
				Param.mRegisterSpace = registerSpace;

				mUAVParams.push_back(Param);
			}
			else if (resourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_SAMPLER)
			{
				ASSERT(Type == ShaderType::PIXEL_SHADER);

				ShaderSamplerParameter Param;
				Param.mName = shaderVarName;
				Param.mShaderType = Type;
				Param.mBindPoint = bindPoint;
				Param.mRegisterSpace = registerSpace;

				mSamplerParams.push_back(Param);
			}

		}
	}

	D3D12_SHADER_VISIBILITY Shader::GetShaderVisibility(ShaderType Type)
	{
		D3D12_SHADER_VISIBILITY shaderVisibility;
		if (Type == ShaderType::VERTEX_SHADER)
		{
			shaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		}
		else if (Type == ShaderType::PIXEL_SHADER)
		{
			shaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		}
		else if (Type == ShaderType::COMPUTE_SHADER)
		{
			shaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		}
		else
		{
			ASSERT(0);
		}

		return shaderVisibility;
	}

	std::vector<CD3DX12_STATIC_SAMPLER_DESC> Shader::CreateStaticSamplers()
	{
		CD3DX12_STATIC_SAMPLER_DESC PointWrap(
			0, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

		CD3DX12_STATIC_SAMPLER_DESC PointClamp(
			1, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

		CD3DX12_STATIC_SAMPLER_DESC LinearWrap(
			2, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

		CD3DX12_STATIC_SAMPLER_DESC LinearClamp(
			3, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

		CD3DX12_STATIC_SAMPLER_DESC AnisotropicWrap(
			4, // shaderRegister
			D3D12_FILTER_ANISOTROPIC, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
			0.0f,                             // mipLODBias
			8);                               // maxAnisotropy

		CD3DX12_STATIC_SAMPLER_DESC AnisotropicClamp(
			5, // shaderRegister
			D3D12_FILTER_ANISOTROPIC, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
			0.0f,                              // mipLODBias
			8);                                // maxAnisotropy

		CD3DX12_STATIC_SAMPLER_DESC Shadow(
			6, // shaderRegister
			D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, // filter
			D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressW
			0.0f,                               // mipLODBias
			16,                                 // maxAnisotropy
			D3D12_COMPARISON_FUNC_LESS_EQUAL,
			D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);

		CD3DX12_STATIC_SAMPLER_DESC DepthMap(
			7, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_LINEAR,   // filter
			D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressW
			0.0f,
			0,
			D3D12_COMPARISON_FUNC_LESS_EQUAL,
			D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE);


		std::vector<CD3DX12_STATIC_SAMPLER_DESC> StaticSamplers;
		StaticSamplers.push_back(PointWrap);
		StaticSamplers.push_back(PointClamp);
		StaticSamplers.push_back(LinearWrap);
		StaticSamplers.push_back(LinearClamp);
		StaticSamplers.push_back(AnisotropicWrap);
		StaticSamplers.push_back(AnisotropicClamp);
		StaticSamplers.push_back(Shadow);
		StaticSamplers.push_back(DepthMap);

		return StaticSamplers;

	}

	void Shader::CreateRootSignature()
	{
		std::vector<CD3DX12_ROOT_PARAMETER> slotRootParameter;

		mCBVSignatureBaseBindSlot = (unsigned)slotRootParameter.size();

		for (const ShaderCBVParameter& param : mCBVParams)
		{
			CD3DX12_ROOT_PARAMETER rootParameter;
			rootParameter.InitAsConstantBufferView(param.mBindPoint, param.mRegisterSpace, GetShaderVisibility(param.mShaderType));
			slotRootParameter.push_back(rootParameter);
		}

		for(const ShaderSRVParameter& param : mSRVParams)
		{
			mSRVCount += param.mBindCount;
		}

		if(mSRVCount > 0)
		{
			mSRVSignatureBaseBindSlot = (unsigned)slotRootParameter.size();

			CD3DX12_DESCRIPTOR_RANGE SRVTable;
			SRVTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, mSRVCount, 0, 0);

			CD3DX12_ROOT_PARAMETER rootParameter;
			rootParameter.InitAsDescriptorTable(1, &SRVTable);
			slotRootParameter.push_back(rootParameter);
		}

		for(const ShaderUAVParameter& param : mUAVParams)
		{
			mUAVCount += param.mBindCount;
		}

		if(mUAVCount > 0)
		{
			mUAVSignatureBaseBindSlot = (unsigned)slotRootParameter.size();

			CD3DX12_DESCRIPTOR_RANGE UAVTable;
			UAVTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, mUAVCount, 0, 0);

			CD3DX12_ROOT_PARAMETER rootParameter;
			rootParameter.InitAsDescriptorTable(1, &UAVTable);
			slotRootParameter.push_back(rootParameter);
		}

		auto staticSamples = CreateStaticSamplers();

		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc((unsigned)slotRootParameter.size(), slotRootParameter.data(),
			(unsigned)staticSamples.size(), staticSamples.data(),
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

		HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
			serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

		if (errorBlob != nullptr)
		{
			::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		}
		ThrowIfFailed(hr, "");

		ThrowIfFailed(mD3D12RHI->GetDevice()->GetNativeDevice()->CreateRootSignature(
			0,
			serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(mRootSignature.ReleaseAndGetAddressOf())), "");
	}

	void Shader::CheckBindings()
	{
		for (ShaderCBVParameter& param : mCBVParams)
		{
			ASSERT(param.mConstantBufferRef != nullptr);
		}

		for (ShaderSRVParameter& param : mSRVParams)
		{
			ASSERT(param.mSRVList.size() > 0);
		}

		for (ShaderUAVParameter& param : mUAVParams)
		{
			ASSERT(param.mUAVList.size() > 0);
		}
	}

	void Shader::ClearBindings()
	{
		for (ShaderCBVParameter& param : mCBVParams)
		{
			param.mConstantBufferRef = nullptr;
		}

		for (ShaderSRVParameter& param : mSRVParams)
		{
			param.mSRVList.clear();
		}

		for (ShaderUAVParameter& param : mUAVParams)
		{
			param.mUAVList.clear();
		}
	}
}
