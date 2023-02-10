#pragma once
#include <filesystem>
#include <unordered_map>
#include "EngineEditor/include/Application/SubSystem/AssetsFileSystem.h"
#include "EngineRuntime/include/Framework/Interface/ISerializable.h"

namespace Editor
{
	class Loader
	{
	public:
		Loader() = default;

		virtual ~Loader() = default;

	public:
		virtual void Load(const std::filesystem::path& modelPath, const std::filesystem::path& outDir, AssetFile* folder) = 0;
	};

	class AssetTool
	{
	public:
		AssetTool();

		bool static LoadAsset(const std::filesystem::path& oldPath, const std::filesystem::path& loadFolder, AssetFile* folder);

		//void static SaveAsset(const std::filesystem::path& saveProjectPath, Engine::ISerializable* asset);

	private:
		std::unordered_map<std::string, std::shared_ptr<Loader>> mExtensionMap;

		static AssetTool staticLoader;
	};
}
