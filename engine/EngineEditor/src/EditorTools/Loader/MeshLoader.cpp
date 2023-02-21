#include <assimp/scene.h>
#include <assimp/Importer.hpp> 
#include <assimp/postprocess.h>
#include <stb/stb_image.h>
#include "EngineEditor/include/EditorTools/Loader/MeshLoader.h"
#include "EngineEditor/include/EditorTools/Loader/TextureLoader.h"
#include "EngineEditor/include/Application/EngineEditor.h"
#include "EngineRuntime/include/Function/Framework/Component/MeshRendererComponent.h"
#include "EngineRuntime/include/Function/Framework/Object/Actor.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/MaterialData.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/MeshData.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/TextureData.h"

namespace Editor
{
	using namespace Engine;

	void LoadMesh(const aiScene* scene, Engine::Mesh& mesh);

	void LoadActor(aiNode* node, Engine::Actor* actorNode, const Engine::Mesh& mesh);

	Engine::GUID MeshLoader::Load(const std::filesystem::path& src, AssetFile* dstFolder)
	{
		std::shared_ptr<Blob> modelFile = NativeFileSystem::GetInstance()->ReadFile(src);
		if (modelFile == nullptr)
		{
			LOG_ERROR("文件不存在");
			return Engine::GUID();
		}

		std::string fileName = src.stem().generic_string();

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFileFromMemory(modelFile->GetData(), modelFile->GetSize(),
			aiProcess_Triangulate | aiProcess_GenBoundingBoxes | aiProcess_ConvertToLeftHanded | aiProcess_SortByPType | aiProcess_CalcTangentSpace);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // 如果不是0
		{
			LOG_ERROR("错误::ASSIMP:: {0}", importer.GetErrorString());
			return Engine::GUID();
		}

		Mesh mesh;
		Actor actor;

		AssetsFileSystem* fileSystem = EngineEditor::GetInstance()->GetFileSystem();

		if (scene->HasMeshes())
		{
			LoadMesh(scene, mesh);
			SerializerDataFrame frame;
			frame << mesh;
			MeshMeta meta;
			meta.SetGuid(mesh.GetGuid());

			fileSystem->CreateAssetFile(dstFolder, fileName + ".mesh", frame, meta);
			LOG_INFO("导入模型 {0}, 保存路径 {1}", fileName.c_str(), dstFolder->GetAssetFilePath().generic_string().c_str());
		}

		std::vector<Engine::GUID> mRefTexture;

		if (scene->HasTextures() && false)
		{
			LOG_INFO("有贴图数据");
			int textureCount = scene->mNumTextures;
			for (int i = 0; i < textureCount; ++i)
			{
				aiTexture* m_texture = scene->mTextures[i];
				std::shared_ptr<TextureData> texture_data;

				std::filesystem::path f(m_texture->mFilename.C_Str());

				LOG_INFO("贴图名: {0} , 宽: {1}, 高: {2}", m_texture->mFilename.C_Str(), m_texture->mWidth, m_texture->mHeight);
				
				if (m_texture->mHeight == 0)
				{
					mRefTexture.emplace_back(TextureLoader::LoadForMemory(m_texture->pcData, m_texture->mWidth, f.filename().generic_string() + ".texture", dstFolder));
				}
				else
				{
					ASSERT(0);
				}
			}
		}

		if (scene->HasAnimations())
		{
			LOG_INFO("有动画数据");
		}

		if (scene->hasSkeletons())
		{
			LOG_INFO("有骨骼数据");
		}
		
		if (scene->mRootNode != nullptr)
		{
			actor.SetActorName(fileName);
			actor.AddComponent<TransformComponent>();
			LoadActor(scene->mRootNode, &actor, mesh);
			SerializerDataFrame frame;
			frame << actor;
			PrefabMeta meta;
			meta.SetGuid(Engine::GUID::Get());

			fileSystem->CreateAssetFile(dstFolder, (fileName + ".prefab"), frame, meta);
			LOG_INFO("导入预制体 {0}, 保存路径 {1}", fileName.c_str(), dstFolder->GetAssetFilePath().generic_string().c_str());
		}
		
		importer.FreeScene();

		return mesh.GetGuid();
	}

	void LoadMesh(const aiScene* scene, Engine::Mesh& mesh)
	{
		unsigned num = scene->mNumMeshes;
		for (unsigned i = 0; i < num; ++i)
		{
			aiMesh& m_mesh = *scene->mMeshes[i];
			
			mesh.Meshes.push_back(SubMesh());
			SubMesh& subMesh = mesh.Meshes.back();
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
					subMesh.Vertices[j].TexCoords.x = m_mesh.mTextureCoords[0][j].x;
					subMesh.Vertices[j].TexCoords.y = m_mesh.mTextureCoords[0][j].y;
				}
				if (m_mesh.mTangents != nullptr)
				{
					subMesh.Vertices[j].Tangent.x = m_mesh.mTangents[j].x;
					subMesh.Vertices[j].Tangent.y = m_mesh.mTangents[j].y;
					subMesh.Vertices[j].Tangent.z = m_mesh.mTangents[j].z;
				}

				if (m_mesh.mBitangents != nullptr)
				{
					subMesh.Vertices[j].Bitangent.x = m_mesh.mBitangents[j].x;
					subMesh.Vertices[j].Bitangent.y = m_mesh.mBitangents[j].y;
					subMesh.Vertices[j].Bitangent.z = m_mesh.mBitangents[j].z;
				}
			}
			
			for (unsigned int j = 0; j < m_mesh.mNumFaces; ++j)
			{
				auto& face = m_mesh.mFaces[j];
				for (unsigned int k = 0; k < face.mNumIndices; ++k)
				{
					subMesh.Indices.push_back(face.mIndices[k]);
				}
			}
			subMesh.mBoundingBox.mMin.x = m_mesh.mAABB.mMin.x;
			subMesh.mBoundingBox.mMin.y = m_mesh.mAABB.mMin.y;
			subMesh.mBoundingBox.mMin.z = m_mesh.mAABB.mMin.z;
			subMesh.mBoundingBox.mMax.x = m_mesh.mAABB.mMax.x;
			subMesh.mBoundingBox.mMax.y = m_mesh.mAABB.mMax.y;
			subMesh.mBoundingBox.mMax.z = m_mesh.mAABB.mMax.z;
			subMesh.MaterialIndex = m_mesh.mMaterialIndex;
			subMesh.mGuid = Engine::GUID::Get();
		}
		mesh.SetGuid(Engine::GUID::Get());
	}

	void LoadActor(aiNode* node, Engine::Actor* actorNode, const Engine::Mesh& mesh)
	{
		unsigned childrenNum = node->mNumChildren;
		Matrix4x4 m;
		memcpy(&m, &node->mTransformation, sizeof(m));
		m = m.Transpose();
		Vector3 position;
		Vector3 scale;
		Quaternion rotation;
		m.Decomposition(position, scale, rotation);
		TransformComponent* transform_component = actorNode->GetComponent<TransformComponent>();
		transform_component->Init(position, scale, rotation);

		if (node->mNumMeshes > 0)
		{
			LOG_INFO("该节点 {0} 有模型数据 添加MeshComponent", node->mName.C_Str());
			MeshRendererComponent* renderComponent = actorNode->AddComponent<Engine::MeshRendererComponent>();
			renderComponent->SetRefMesh(mesh.GetGuid());
		}

		for (unsigned i = 0; i < childrenNum; ++i)
		{
			Actor* childrenActor = new Actor();
			childrenActor->SetActorName(node->mChildren[i]->mName.C_Str());
			actorNode->AddChildrenForTools(childrenActor);
			childrenActor->AddComponent<TransformComponent>();
			LoadActor(node->mChildren[i], childrenActor, mesh);
		}

	}

}
