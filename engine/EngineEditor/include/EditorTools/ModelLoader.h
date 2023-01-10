#pragma once
#include <filesystem>
#include <assimp/scene.h>
#include "EngineEditor/include/EditorTools/AssetTool.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/MeshData.h"
#include "EngineRuntime/include/Function/Framework/Object/Actor.h"

namespace Editor
{
	class ModelLoader : public Loader
	{
	public:
		ModelLoader() = default;

		virtual ~ModelLoader() override = default;

	public:
		virtual void Load(const std::filesystem::path& modelPath, const std::filesystem::path& outDir, AssetFile* folder) override;

	private:
		void LoadMesh(const aiScene* scene, Engine::Mesh& mesh);

		void LoadMaterial(const aiScene* scene);

		void LoadActor(aiNode* node, Engine::Actor* actorNode);
	};
}
