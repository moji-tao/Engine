#include <algorithm>
#include "EngineEditor/include/EditorTools/AssetTool.h"
#include "EngineEditor/include/EditorTools/ModelLoader.h"
#include "EngineRuntime/include/Core/Meta/Serializer.h"
#include "EngineRuntime/include/Resource/AssetManager/AssetManager.h"

namespace Editor
{
	AssetTool AssetTool::staticLoader;

	AssetTool::AssetTool()
	{
		std::shared_ptr<ModelLoader> modelLoader = std::make_shared<ModelLoader>();
		mExtensionMap[".fbx"] = modelLoader;

	}

	bool AssetTool::LoadAsset(const std::filesystem::path& oldPath, const std::filesystem::path& loadFolder, AssetFile* folder)
	{
		std::string extension = oldPath.extension().generic_string();
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

		auto it = staticLoader.mExtensionMap.find(extension);
		if (it == staticLoader.mExtensionMap.end())
		{
			LOG_ERROR("未知类型, 导入失败  {0}", extension.c_str());
			return false;
		}

		it->second->Load(oldPath, loadFolder, folder);

		return true;
	}

	void AssetTool::SaveAsset(const std::filesystem::path& saveProjectPath, Engine::ISerializable* asset)
	{
		Engine::SerializerDataFrame frame;
		//asset->Serialize(frame);
		frame << *asset;
		frame.Save(saveProjectPath);

		//Engine::AssetManager::GetInstance()->AddAssetUrl(saveProjectPath);
	}
}


