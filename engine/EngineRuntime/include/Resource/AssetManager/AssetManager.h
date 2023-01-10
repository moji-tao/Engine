#pragma once
#include <memory>
#include <filesystem>
#include "EngineRuntime/include/Core/Random/Random.h"
#include "EngineRuntime/include/Framework/Interface/ISingleton.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/MaterialData.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/SkeletonData.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/TextureData.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/MeshData.h"

namespace Engine
{
	class Actor;
	class Level;

	struct MeshAssetTrait { };
	struct TextureAssetTrait { };
	struct MaterialAssetTrait { };
	struct PrefabAssetTrait { };
	struct SceneAssetTrait { };
	struct SkeletonAssetTrait { };

	class AssetManager : public ISingleton<AssetManager>
	{
	public:
		bool Initialize();

		void Finalize();

	public:
		void AddAssetUrl(const std::filesystem::path& assetUrl);

		void AddMeshAssetUrl(const std::filesystem::path& assetUrl);
		void DeleteMeshAssetUrl(const std::filesystem::path& assetUrl);
		void AddAssetUrl(const std::filesystem::path& assetUrl, const MeshAssetTrait&);

		void AddTextureAssetUrl(const std::filesystem::path& assetUrl);
		void DeleteTextureAssetUrl(const std::filesystem::path& assetUrl);
		void AddAssetUrl(const std::filesystem::path& assetUrl, const TextureAssetTrait&);

		void AddMaterialAssetUrl(const std::filesystem::path& assetUrl);
		void DeleteMaterialAssetUrl(const std::filesystem::path& assetUrl);
		void AddAssetUrl(const std::filesystem::path& assetUrl, const MaterialAssetTrait&);

		void AddPrefabAssetUrl(const std::filesystem::path& assetUrl);
		void DeletePrefabAssetUrl(const std::filesystem::path& assetUrl);
		void AddAssetUrl(const std::filesystem::path& assetUrl, const PrefabAssetTrait&);

		void AddSceneAssetUrl(const std::filesystem::path& assetUrl);
		void DeleteSceneAssetUrl(const std::filesystem::path& assetUrl);
		void AddAssetUrl(const std::filesystem::path& assetUrl, const SceneAssetTrait&);

		void AddSkeletonAssetUrl(const std::filesystem::path& assetUrl);
		void DeleteSkeletonAssetUrl(const std::filesystem::path& assetUrl);
		void AddAssetUrl(const std::filesystem::path& assetUrl, const SkeletonAssetTrait&);

		void DeleteAssetUrl(const std::filesystem::path& assetUrl);
		void DeleteAssetFolder(const std::filesystem::path& assetUrl);

	public:
		void LoadAssertConfig();

		void SaveAssertConfig();

	private:
		std::unordered_map<std::filesystem::path, Mesh*> mMeshAsset;
		std::unordered_map<std::filesystem::path, TextureData*> mTextureAsset;
		//std::unordered_map<std::filesystem::path, Mesh*> mFontAsset;
		std::unordered_map<std::filesystem::path, MaterialData*> mMaterialAsset;
		std::unordered_map<std::filesystem::path, Actor*> mPrefabAsset;
		std::unordered_map<std::filesystem::path, Level*> mSceneAsset;
		std::unordered_map<std::filesystem::path, SkeletonData*> mSkeletonAsset;
	};
}
