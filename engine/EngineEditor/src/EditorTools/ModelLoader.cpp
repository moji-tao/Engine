#include <assimp/Importer.hpp> 
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "EngineEditor/include/EditorTools/ModelLoader.h"
#include "EngineRuntime/include/Platform/FileSystem/FileSystem.h"
#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Function/Framework/Component/TransformComponent.h"
#include "EngineRuntime/include/Core/Meta/Serializer.h"
#include "EngineRuntime/include/Core/Random/Random.h"
#include "EngineRuntime/include/Resource/AssetManager/AssetManager.h"

namespace Editor
{
	using namespace Engine;

	void ModelLoader::Load(const std::filesystem::path& modelPath, const std::filesystem::path& outDir, AssetFile* folder)
	{
		std::shared_ptr<Blob> modelFile = NativeFileSystem::GetInstance()->ReadFile(modelPath);
		if (modelFile == nullptr)
		{
			LOG_ERROR("文件不存在");
			return;
		}

		std::string fileName = modelPath.stem().generic_string();
		
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFileFromMemory(modelFile->GetData(), modelFile->GetSize(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // 如果不是0
		{
			LOG_ERROR("错误::ASSIMP:: {0}", importer.GetErrorString());
			return;
		}

		Mesh mesh;
		Actor actor;
		
		if (scene->HasMeshes())
		{
			std::filesystem::path outPath = outDir / (fileName + ".mesh");
			LoadMesh(scene, mesh);
			SerializerDataFrame frame;
			frame << mesh;
			frame.Save(outPath);
			Engine::GUID guid = Engine::Random::GetGUID();
			//Engine::AssetManager::GetInstance()->AddMeshAssetUrl(outPath);

			std::shared_ptr<AssetFile> file = std::make_shared<AssetFile>();
			file->mName = fileName + ".mesh";
			file->mParent = folder;
			file->mIsFolder = false;
			folder->next.push_back(file);
			LOG_INFO("导入模型 {0}, GUID: {1}, 保存路径 {2}", fileName.c_str(), guid.c_str(), outPath.generic_string().c_str());
		}

		if (scene->HasMaterials())
		{
			//LoadMaterial(scene);
		}

		if (scene->HasTextures())
		{
			LOG_INFO("有贴图数据");
		}

		if (scene->mRootNode != nullptr)
		{
			std::filesystem::path outPath = outDir / (fileName + ".prefab");
			actor.SetActorName(fileName);
			actor.AddComponent<TransformComponent>();
			LoadActor(scene->mRootNode, &actor);
			SerializerDataFrame frame;
			frame << actor;
			frame.Save(outPath);
			Engine::GUID guid = Engine::Random::GetGUID();
			//Engine::AssetManager::GetInstance()->AddPrefabAssetUrl(outPath);

			std::shared_ptr<AssetFile> file = std::make_shared<AssetFile>();
			file->mName = fileName + ".prefab";
			file->mParent = folder;
			file->mIsFolder = false;
			folder->next.push_back(file);
			LOG_INFO("导入预制体 {0}, GUID: {1}, 保存路径 {2}", fileName.c_str(), guid.c_str(), outPath.generic_string().c_str());
		}
	}

	void ModelLoader::LoadMesh(const aiScene* scene, Engine::Mesh& mesh)
	{
		unsigned num = scene->mNumMeshes;
		for (unsigned i = 0; i < num; ++i)
		{
			aiMesh& m_mesh = *scene->mMeshes[i];
			SubMesh subMesh;
			subMesh.Vertices.resize(m_mesh.mNumVertices);
			for (unsigned int j = 0; j < m_mesh.mNumVertices; ++j)
			{
				subMesh.Vertices[j].Position.x = m_mesh.mVertices[j].x;
				subMesh.Vertices[j].Position.y = m_mesh.mVertices[j].y;
				subMesh.Vertices[j].Position.z = m_mesh.mVertices[j].z;

				subMesh.Vertices[j].Normal.x = m_mesh.mNormals[j].x;
				subMesh.Vertices[j].Normal.y = m_mesh.mNormals[j].y;
				subMesh.Vertices[j].Normal.z = m_mesh.mNormals[j].z;

				if (m_mesh.mTextureCoords[0])
				{
					subMesh.Vertices[j].TexCoords.x = m_mesh.mTextureCoords[0]->x;
					subMesh.Vertices[j].TexCoords.y = m_mesh.mTextureCoords[0]->y;
				}

				subMesh.Vertices[j].Tangent.x = m_mesh.mTangents[j].x;
				subMesh.Vertices[j].Tangent.y = m_mesh.mTangents[j].y;
				subMesh.Vertices[j].Tangent.z = m_mesh.mTangents[j].z;

				subMesh.Vertices[j].Bitangent.x = m_mesh.mBitangents[j].x;
				subMesh.Vertices[j].Bitangent.y = m_mesh.mBitangents[j].y;
				subMesh.Vertices[j].Bitangent.z = m_mesh.mBitangents[j].z;
			}

			for (unsigned int j = 0; j < m_mesh.mNumFaces; ++j)
			{
				for (unsigned int k = 0; k < m_mesh.mFaces->mNumIndices; ++k)
				{
					subMesh.Indices.push_back(m_mesh.mFaces->mIndices[k]);
				}
			}
			subMesh.MaterialIndex = m_mesh.mMaterialIndex;
			
			mesh.Meshes.push_back(subMesh);
		}
	}

	void ModelLoader::LoadMaterial(const aiScene* scene)
	{
		for (unsigned i = 0; i < scene->mNumMaterials; ++i)
		{
			aiMaterial& material = *scene->mMaterials[i];
			LOG_INFO(material.GetName().C_Str());
			//material.Get(AI_MATKEY_).
			//material.GetTexture(aiT)
			aiString s;
			material.GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &s);
			
			LOG_INFO(s.C_Str());
		}
	}

	void ModelLoader::LoadActor(aiNode* node, Engine::Actor* actorNode)
	{
		unsigned childrenNum = node->mNumChildren;
		Matrix4x4 m;
		memcpy(&m, &node->mTransformation, sizeof(m));
		Vector3 position;
		Vector3 scale;
		Quaternion rotation;
		m.Decomposition(position, scale, rotation);
		TransformComponent* transform_component = actorNode->GetTransform();
		transform_component->SetPosition(position);
		transform_component->SetScale(scale);
		transform_component->SetQuaternion(rotation);

		for (unsigned i = 0; i < childrenNum; ++i)
		{
			Actor* childrenActor = new Actor();
			childrenActor->AddComponent<TransformComponent>();
			childrenActor->SetActorName(node->mChildren[i]->mName.C_Str());
			actorNode->AddChildrenForTools(childrenActor);

			LoadActor(node->mChildren[i], childrenActor);
		}

	}
}
