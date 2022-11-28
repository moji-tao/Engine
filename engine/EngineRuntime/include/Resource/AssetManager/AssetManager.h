#pragma once
#include <memory>
#include "EngineRuntime/include/Framework/Interface/ISingleton.h"
#include "EngineRuntime/include/Resource/AssetManager/MeshAssetManager.h"
#include "EngineRuntime/include/Resource/AssetManager/TextureAssetManager.h"

namespace Engine
{
	class AssetManager : public ISingleton<AssetManager>
	{
	public:
		bool Initialize();

		void Finalize();

	public:
		const MeshAssetManager* GetMeshAssetManager() const;

		const TextureAssetManager* GetTextureAssetManager() const;

	private:
		std::unique_ptr<MeshAssetManager> mMeshAsset;

		std::unique_ptr<TextureAssetManager> mTextureAsset;
	};
}
