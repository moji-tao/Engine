#include <cassert>
#include "EngineRuntime/include/Resource/AssetManager/AssetManager.h"

namespace Engine
{
	bool AssetManager::Initialize()
	{
		mMeshAsset = std::make_unique<MeshAssetManager>();

		mTextureAsset = std::unique_ptr<TextureAssetManager>();

		return true;
	}

	void AssetManager::Finalize()
	{
		mMeshAsset.reset();

		mTextureAsset.reset();
	}

	const MeshAssetManager* AssetManager::GetMeshAssetManager() const
	{
		assert(mMeshAsset != nullptr);

		return mMeshAsset.get();
	}

	const TextureAssetManager* AssetManager::GetTextureAssetManager() const
	{
		assert(mTextureAsset != nullptr);

		return mTextureAsset.get();
	}
}
