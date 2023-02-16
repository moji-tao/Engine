#pragma once
#include <filesystem>
#include "EngineEditor/include/Application/SubSystem/AssetsFileSystem.h"

namespace Editor
{
	class TextureLoader
	{
	public:
		static Engine::GUID Load(const std::filesystem::path& src, AssetFile* dstFolder);

		static Engine::GUID LoadHDR(const std::filesystem::path& src, AssetFile* dstFolder);

		static Engine::GUID LoadForMemory(const void* data, uint64_t size, const std::string& fileName, AssetFile* dstFolder);
	};
}
