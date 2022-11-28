#pragma once
#include <unordered_map>
#include <string>
#include <filesystem>
#include "EngineRuntime/include/Resource/ResourceType/Data/MeshData.h"

namespace Engine
{
	class MeshAssetManager
	{
	public:
		const GeometryData* GetMesh(const std::filesystem::path& assetUrl);

	private:
		bool LoadMeshAsset(const std::filesystem::path& assetUrl);

	private:
		std::unordered_map<std::string, std::unique_ptr<GeometryData>> mModelSet;
	};
}
