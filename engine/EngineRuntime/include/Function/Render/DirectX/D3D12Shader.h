#pragma once
#include "EngineRuntime/include/Function/Render/DirectX/D3D12Buffer.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12RHI.h"
#include "EngineRuntime/include/Platform/FileSystem/FileSystem.h"

namespace Engine
{
	enum class ShaderType
	{
		VERTEX_SHADER,
		GEOMETRY_SHADER,
		PIXEL_SHADER,
		COMPUTE_SHADER
	};

	struct ShaderParameter
	{
		std::string mName;
		ShaderType mShaderType;
		unsigned mBindPoint;
		unsigned mRegisterSpace;
	};

	struct ShaderCBVParameter : ShaderParameter
	{
		D3D12ConstantBufferRef mConstantBufferRef;
	};

	struct ShaderSRVParameter : ShaderParameter
	{
		unsigned mBindCount;

		std::vector<D3D12ShaderResourceView*> mSRVList;
	};

	struct ShaderUAVParameter : ShaderParameter
	{
		unsigned mBindCount;

		std::vector<D3D12UnorderedAccessView*> mUAVList;
	};

	struct ShaderSamplerParameter : ShaderParameter
	{
		
	};

	struct ShaderInfo
	{
		std::shared_ptr<Blob> mVertexShader;

		std::shared_ptr<Blob> mGeometryShader;

		std::shared_ptr<Blob> mPixelShader;

		std::shared_ptr<Blob> mComputeShader;
	};

	class Shader
	{
	public:
		Shader(const ShaderInfo& InShaderInfo, D3D12RHI* InD3D12RHI);

		bool SetParameter(const std::string& ParamName, D3D12ConstantBufferRef ConstantBufferRef);

		bool SetParameter(const std::string& ParamName, D3D12ShaderResourceView* SRV);

		bool SetParameter(const std::string& ParamName, const std::vector<D3D12ShaderResourceView*>& SRVList);

		bool SetParameter(const std::string& ParamName, D3D12UnorderedAccessView* UAV);

		bool SetParameter(const std::string& ParamName, const std::vector<D3D12UnorderedAccessView*>& UAVList);

		void BindParameters();

	private:
		void Initialize();

		void GetShaderParameters(std::shared_ptr<Blob> ShaderBlob, ShaderType Type);

		D3D12_SHADER_VISIBILITY GetShaderVisibility(ShaderType Type);

		std::vector<CD3DX12_STATIC_SAMPLER_DESC> CreateStaticSamplers();

		void CreateRootSignature();

		void CheckBindings();

		void ClearBindings();

	public:
		ShaderInfo mShaderInfo;

		std::vector<ShaderCBVParameter> mCBVParams;

		std::vector<ShaderSRVParameter> mSRVParams;

		std::vector<ShaderUAVParameter> mUAVParams;

		std::vector<ShaderSamplerParameter> mSamplerParams;

		unsigned mCBVSignatureBaseBindSlot;

		unsigned mSRVSignatureBaseBindSlot;

		unsigned mUAVSignatureBaseBindSlot;

		unsigned mSamplerSignatureBaseBindSlot;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;

		unsigned mSRVCount = 0;

		unsigned mUAVCount = 0;

	private:
		D3D12RHI* mD3D12RHI = nullptr;
	};
}
