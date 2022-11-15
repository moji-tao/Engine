#include "EngineRuntime/include/Platform/FileSystem/FileSystem.h"
#include "EngineRuntime/include/Function/Render/ShaderUtil.h"

#include <nvrhi/common/shader-blob.h>

#include <d3dcompiler.h>
#include <wrl/client.h>

namespace Engine
{
	nvrhi::ShaderHandle ShaderUtil::CreateShaderFormEngineFolder(nvrhi::IDevice* device, const std::filesystem::path& shaderPath, const char* entryName, const std::vector<ShaderMacro>* pDefines, nvrhi::ShaderType shaderType)
	{
		nvrhi::ShaderDesc desc = nvrhi::ShaderDesc(shaderType);
		desc.debugName = shaderPath.generic_string();
		desc.entryName = entryName;
		return CreateShaderFormEngineFolder(device, shaderPath, pDefines, desc);
	}

	nvrhi::ShaderHandle ShaderUtil::CreateShaderFormEngineFolder(nvrhi::IDevice* device, const std::filesystem::path& shaderPath, const std::vector<ShaderMacro>* pDefines, const nvrhi::ShaderDesc& desc)
	{
		std::shared_ptr<Blob> blob = EngineFileSystem::GetInstance()->ReadFile(shaderPath);

		if (!blob)
			return nullptr;

		std::vector<nvrhi::ShaderConstant> constants;
		if (pDefines)
		{
			for (const ShaderMacro& define : *pDefines)
				constants.push_back(nvrhi::ShaderConstant{ define.name.c_str(), define.definition.c_str() });
		}
		
		return nvrhi::createShaderPermutation(device, desc, blob->GetData(), blob->GetSize(),
			constants.data(), uint32_t(constants.size()));
	}

	nvrhi::ShaderHandle ShaderUtil::CreateShaderFormProjectFolder(nvrhi::IDevice* device, const std::filesystem::path& shaderPath, const char* entryName, const std::vector<ShaderMacro>* pDefines, nvrhi::ShaderType shaderType)
	{
		nvrhi::ShaderDesc desc = nvrhi::ShaderDesc(shaderType);
		desc.debugName = shaderPath.generic_string();
		desc.entryName = entryName;
		return CreateShaderFormProjectFolder(device, shaderPath, pDefines, desc);
	}

	nvrhi::ShaderHandle ShaderUtil::CreateShaderFormProjectFolder(nvrhi::IDevice* device, const std::filesystem::path& shaderPath, const std::vector<ShaderMacro>* pDefines, const nvrhi::ShaderDesc& desc)
	{
		std::shared_ptr<Blob> blob = ProjectFileSystem::GetInstance()->ReadFile(shaderPath);

		if (!blob)
			return nullptr;

		std::vector<nvrhi::ShaderConstant> constants;
		if (pDefines)
		{
			for (const ShaderMacro& define : *pDefines)
				constants.push_back(nvrhi::ShaderConstant{ define.name.c_str(), define.definition.c_str() });
		}

		return nvrhi::createShaderPermutation(device, desc, blob->GetData(), blob->GetSize(),
			constants.data(), uint32_t(constants.size()));
	}

	nvrhi::ShaderHandle ShaderUtil::CompileShader(nvrhi::IDevice* device, const std::filesystem::path& shaderPath, const char* enterName, nvrhi::ShaderType shaderType)
	{
		unsigned compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errors;
		const char* target;
		if(shaderType == nvrhi::ShaderType::Vertex)
		{
			target = "vs_5_1";
		}
		else
		{
			target = "ps_5_1";
		}
		HRESULT hr = D3DCompileFromFile(shaderPath.generic_wstring().c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, enterName, target,
			compileFlags, 0, byteCode.GetAddressOf(), errors.GetAddressOf());

		if (errors != nullptr)
		{
			return nullptr;
		}
		nvrhi::ShaderDesc desc;
		desc.entryName = enterName;
		desc.shaderType = shaderType;
		std::vector<nvrhi::ShaderConstant> constants;

		return nvrhi::createShaderPermutation(device, desc, byteCode->GetBufferPointer(), byteCode->GetBufferSize(),
			constants.data(), uint32_t(constants.size()));

	}

}
