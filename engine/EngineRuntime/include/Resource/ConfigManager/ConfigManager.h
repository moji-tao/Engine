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

	private:
		//std::filesystem::path
	};
}
