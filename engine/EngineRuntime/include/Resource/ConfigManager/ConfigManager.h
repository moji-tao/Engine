#pragma once
#include "EngineRuntime/include/Framework/Interface/ISingleton.h"
#include <filesystem>

namespace Engine
{
	class ConfigManager : public ISingleton<ConfigManager>
	{
	public:
		bool Initialize(const std::filesystem::path& enginePath);

		void Finalize();

	public:
		std::filesystem::path GetDefaultScenePath();

		void SetDefaultScenePath(const std::filesystem::path& defaultPath);

	private:
		void LoadAssertConfig();

		void SaveAssertConfig();

	private:
		std::filesystem::path mCurrentDefaultScenePath;
	};
}
