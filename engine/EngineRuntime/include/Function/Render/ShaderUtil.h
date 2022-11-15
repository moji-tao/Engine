#pragma once
#include <filesystem>

#include <nvrhi/nvrhi.h>

namespace Engine
{
	struct ShaderMacro
	{
		std::string name;
		std::string definition;

		ShaderMacro(const std::string& _name, const std::string& _definition)
			: name(_name)
			, definition(_definition)
		{ }
	};

	class ShaderUtil
	{
	public:
		static nvrhi::ShaderHandle CreateShaderFormEngineFolder(nvrhi::IDevice* device, const std::filesystem::path& shaderPath, const char* entryName, const std::vector<ShaderMacro>* pDefines, nvrhi::ShaderType shaderType);

		static nvrhi::ShaderHandle CreateShaderFormEngineFolder(nvrhi::IDevice* device, const std::filesystem::path& shaderPath, const std::vector<ShaderMacro>* pDefines, const nvrhi::ShaderDesc& desc);

		static nvrhi::ShaderHandle CreateShaderFormProjectFolder(nvrhi::IDevice* device, const std::filesystem::path& shaderPath, const char* entryName, const std::vector<ShaderMacro>* pDefines, nvrhi::ShaderType shaderType);

		static nvrhi::ShaderHandle CreateShaderFormProjectFolder(nvrhi::IDevice* device, const std::filesystem::path& shaderPath, const std::vector<ShaderMacro>* pDefines, const nvrhi::ShaderDesc& desc);

		static nvrhi::ShaderHandle CompileShader(nvrhi::IDevice* device, const std::filesystem::path& shaderPath, const char* enterName, nvrhi::ShaderType shaderType);
	};
}
