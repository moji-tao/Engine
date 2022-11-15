#pragma once
#include "EngineRuntime/include/Resource/ConfigManager/ConfigManager.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/MeshData.h"
#include "EngineRuntime/include/Core/Random/Random.h"
#include <unordered_map>



namespace Engine
{

	class AssetManager : public ISingleton<AssetManager>
	{
	public:
		bool Initialize();

		void Finalize();

	public:
		GUID LoadAsset(const std::filesystem::path& assetUrl);

		std::vector<Mesh>* GetAsset(const GUID& guid);

	private:

	private:
		std::unordered_map<GUID, std::vector<Mesh>> mModelSet;
	};
}
