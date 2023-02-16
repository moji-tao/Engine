#pragma once
#include <filesystem>
#include "EngineEditor/include/Application/SubSystem/AssetsFileSystem.h"

namespace Editor
{
	class MeshLoader
	{
	public:
		static Engine::GUID Load(const std::filesystem::path& src, AssetFile* dstFolder);

	};
}
