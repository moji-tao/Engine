#include <algorithm>
#include "EngineEditor/include/EditorTools/AssetTool.h"
#include "EngineEditor/include/EditorTools/Loader/TextureLoader.h"
#include "EngineEditor/include/EditorTools/Loader/MeshLoader.h"
#include "EngineRuntime/include/Core/Meta/Serializer.h"
#include "EngineRuntime/include/Resource/AssetManager/AssetManager.h"

namespace Editor
{
	AssetTool AssetTool::staticLoader;

	AssetTool::AssetTool()
	{
		mExtensionMap[".png"] = TextureLoader::Load;
		mExtensionMap[".jpg"] = TextureLoader::Load;
		mExtensionMap[".tga"] = TextureLoader::Load;
		mExtensionMap[".hdr"] = TextureLoader::LoadHDR;
		mExtensionMap[".fbx"] = MeshLoader::Load;
	}

	bool AssetTool::LoadAsset(const std::filesystem::path& oldPath, AssetFile* folder)
	{
		std::string extension = oldPath.extension().generic_string();
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

		auto it = staticLoader.mExtensionMap.find(extension);
		if (it == staticLoader.mExtensionMap.end())
		{
			LOG_ERROR("未知类型, 导入失败  {0}", extension.c_str());
			return false;
		}

		Engine::GUID guid = it->second(oldPath, folder);

		return guid.IsVaild();
	}
}


