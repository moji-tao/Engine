#include <fstream>
#include <json/json.h>
#include "EngineRuntime/include/Resource/AssetManager/AssetManager.h"
#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Platform/FileSystem/FileSystem.h"

namespace Engine
{
	bool AssetManager::Initialize()
	{
		//LoadAssertConfig();

		return true;
	}

	void AssetManager::Finalize()
	{
		//SaveAssertConfig();
	}

	void AssetManager::AddAssetUrl(const std::filesystem::path& assetUrl)
	{
		std::string extension = assetUrl.extension().generic_string();

		if (extension == ".mesh")
		{
			AddMeshAssetUrl(assetUrl);
		}
		else if (extension == ".texture")
		{
			AddTextureAssetUrl(assetUrl);
		}
		else if (extension == ".material")
		{
			AddMaterialAssetUrl(assetUrl);
		}
		else if (extension == ".prefab")
		{
			AddPrefabAssetUrl(assetUrl);
		}
		else if (extension == ".scene")
		{
			AddSceneAssetUrl(assetUrl);
		}
		else if (extension == ".skeleton")
		{
			AddSkeletonAssetUrl(assetUrl);
		}
		else
		{
			ASSERT("未知类型");
		}
	}

	void AssetManager::AddMeshAssetUrl(const std::filesystem::path& assetUrl)
	{
		ASSERT(mMeshAsset.find(assetUrl) == mMeshAsset.end());

		mMeshAsset.emplace(assetUrl, nullptr);
	}

	void AssetManager::DeleteMeshAssetUrl(const std::filesystem::path& assetUrl)
	{
		ASSERT(mMeshAsset.find(assetUrl) != mMeshAsset.end());

		mMeshAsset.erase(assetUrl);
	}

	void AssetManager::AddAssetUrl(const std::filesystem::path& assetUrl, const MeshAssetTrait&)
	{
		AddMeshAssetUrl(assetUrl);
	}

	void AssetManager::AddTextureAssetUrl(const std::filesystem::path& assetUrl)
	{
		ASSERT(mTextureAsset.find(assetUrl) == mTextureAsset.end());

		mTextureAsset.emplace(assetUrl, nullptr);
	}

	void AssetManager::DeleteTextureAssetUrl(const std::filesystem::path& assetUrl)
	{
		ASSERT(mTextureAsset.find(assetUrl) != mTextureAsset.end());

		mTextureAsset.erase(assetUrl);
	}

	void AssetManager::AddAssetUrl(const std::filesystem::path& assetUrl, const TextureAssetTrait&)
	{
		AddTextureAssetUrl(assetUrl);
	}

	void AssetManager::AddMaterialAssetUrl(const std::filesystem::path& assetUrl)
	{
		ASSERT(mMaterialAsset.find(assetUrl) == mMaterialAsset.end());

		mMaterialAsset.emplace(assetUrl, nullptr);
	}

	void AssetManager::DeleteMaterialAssetUrl(const std::filesystem::path& assetUrl)
	{
		ASSERT(mMaterialAsset.find(assetUrl) != mMaterialAsset.end());

		mMaterialAsset.erase(assetUrl);
	}

	void AssetManager::AddAssetUrl(const std::filesystem::path& assetUrl, const MaterialAssetTrait&)
	{
		AddMaterialAssetUrl(assetUrl);
	}

	void AssetManager::AddPrefabAssetUrl(const std::filesystem::path& assetUrl)
	{
		ASSERT(mPrefabAsset.find(assetUrl) == mPrefabAsset.end());

		mPrefabAsset.emplace(assetUrl, nullptr);
	}

	void AssetManager::DeletePrefabAssetUrl(const std::filesystem::path& assetUrl)
	{
		ASSERT(mPrefabAsset.find(assetUrl) != mPrefabAsset.end());

		mPrefabAsset.erase(assetUrl);
	}

	void AssetManager::AddAssetUrl(const std::filesystem::path& assetUrl, const PrefabAssetTrait&)
	{
		AddPrefabAssetUrl(assetUrl);
	}

	void AssetManager::AddSceneAssetUrl(const std::filesystem::path& assetUrl)
	{
		ASSERT(mSceneAsset.find(assetUrl) == mSceneAsset.end());

		mSceneAsset.emplace(assetUrl, nullptr);
	}

	void AssetManager::DeleteSceneAssetUrl(const std::filesystem::path& assetUrl)
	{
		ASSERT(mSceneAsset.find(assetUrl) != mSceneAsset.end());

		mSceneAsset.erase(assetUrl);
	}

	void AssetManager::AddAssetUrl(const std::filesystem::path& assetUrl, const SceneAssetTrait&)
	{
		AddSceneAssetUrl(assetUrl);
	}

	void AssetManager::AddSkeletonAssetUrl(const std::filesystem::path& assetUrl)
	{
		ASSERT(mSkeletonAsset.find(assetUrl) == mSkeletonAsset.end());

		mSkeletonAsset.emplace(assetUrl, nullptr);
	}

	void AssetManager::DeleteSkeletonAssetUrl(const std::filesystem::path& assetUrl)
	{
		ASSERT(mSkeletonAsset.find(assetUrl) != mSkeletonAsset.end());

		mSkeletonAsset.erase(assetUrl);
	}

	void AssetManager::AddAssetUrl(const std::filesystem::path& assetUrl, const SkeletonAssetTrait&)
	{
		AddSkeletonAssetUrl(assetUrl);
	}

	void AssetManager::DeleteAssetUrl(const std::filesystem::path& assetUrl)
	{
		std::string extension = assetUrl.extension().generic_string();

		if (extension == ".mesh")
		{
			DeleteMeshAssetUrl(assetUrl);
		}
		else if (extension == ".texture")
		{
			DeleteTextureAssetUrl(assetUrl);
		}
		else if (extension == ".material")
		{
			DeleteMaterialAssetUrl(assetUrl);
		}
		else if (extension == ".prefab")
		{
			DeletePrefabAssetUrl(assetUrl);
		}
		else if (extension == ".scene")
		{
			DeleteSceneAssetUrl(assetUrl);
		}
		else if (extension == ".skeleton")
		{
			DeleteSkeletonAssetUrl(assetUrl);
		}
		else
		{
			ASSERT(0, "未知类型");
		}
	}

	void AssetManager::DeleteAssetFolder(const std::filesystem::path& assetUrl)
	{
	}

	template<typename T>
	void LoadJson(const Json::Value& value, std::unordered_map<std::filesystem::path, T*>& mp)
	{
		for (unsigned i = 0; i < value.size(); ++i)
		{
			mp.emplace(value[i].asString(), nullptr);
		}
	}

	void AssetManager::LoadAssertConfig()
	{
		std::shared_ptr<Blob> blob = ProjectFileSystem::GetInstance()->ReadFile("Library/AssetsConfig.json");
		ASSERT(blob != nullptr);

		Json::Reader reader;
		Json::Value root;

		if (reader.parse((char*)blob->GetData(), (char*)blob->GetData() + blob->GetSize(), root))
		{
			LoadJson(root["MeshAsset"], mMeshAsset);
			LoadJson(root["TextureAsset"], mTextureAsset);
			LoadJson(root["MaterialAsset"], mMaterialAsset);
			LoadJson(root["PrefabAsset"], mPrefabAsset);
			LoadJson(root["SceneAsset"], mSceneAsset);
			LoadJson(root["SkeletonAsset"], mSkeletonAsset);
		}
	}

	template<typename T>
	void SaveJson(const std::string& name, const std::unordered_map<std::filesystem::path, T*>& mp, Json::Value& root)
	{
		for (auto it = mp.begin(); it != mp.end(); ++it)
		{
			root[name].append(it->first.generic_string());
		}
	}

	void AssetManager::SaveAssertConfig()
	{
		Json::Value root;
		SaveJson("MeshAsset", mMeshAsset, root);
		SaveJson("TextureAsset", mTextureAsset, root);
		SaveJson("MaterialAsset", mMaterialAsset, root);
		SaveJson("PrefabAsset", mPrefabAsset, root);
		SaveJson("SceneAsset", mSceneAsset, root);
		SaveJson("SkeletonAsset", mSkeletonAsset, root);

		Json::Value def = []() {
			Json::Value def;
			Json::StreamWriterBuilder::setDefaults(&def);
			def["emitUTF8"] = true;
			return def;
		}();

		std::ofstream stream(ProjectFileSystem::GetInstance()->GetActualPath("Library/AssetsConfig.json"));
		Json::StreamWriterBuilder stream_builder;
		stream_builder.settings_ = def;//Config emitUTF8
		std::unique_ptr<Json::StreamWriter> writer(stream_builder.newStreamWriter());
		writer->write(root, &stream);
	}
}
