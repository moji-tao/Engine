#include "EngineRuntime/include/Resource/AssetManager/AssetManager.h"
#include "EngineRuntime/include/Platform/FileSystem/FileSystem.h"
#include "EngineRuntime/include/Core/Base/macro.h"

#include <assimp/Importer.hpp>        //assimp库头文件
#include <assimp/scene.h>
#include <assimp/postprocess.h>



namespace Engine
{
	void processNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& outMeshes);

	void processMesh(aiMesh* mesh, const aiScene* scene, Mesh& outMesh);

	bool AssetManager::Initialize()
	{
		return true;
	}

	void AssetManager::Finalize()
	{

	}

	GUID AssetManager::LoadAsset(const std::filesystem::path& assetUrl)
	{
		std::shared_ptr<Blob> blob = ProjectFileSystem::GetInstance()->ReadFile(assetUrl);
		
		Assimp::Importer importer; 
		const aiScene* scene = importer.ReadFileFromMemory(blob->GetData(), blob->GetSize(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // 如果不是0
		{
			LOG_ERROR("错误::ASSIMP:: {0}", importer.GetErrorString());
			return "";
		}

		GUID guid = Random::GetGUID();

		std::vector<Mesh>& meshes = mModelSet[guid];

		processNode(scene->mRootNode, scene, meshes);

		return guid;
	}

	std::vector<Mesh>* AssetManager::GetAsset(const GUID& guid)
	{
		auto it = mModelSet.find(guid);
		if(it == mModelSet.end())
		{
			return nullptr;
		}
		return &it->second;
	}

	void processNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& outMeshes)
	{
		unsigned int k = outMeshes.size();
		outMeshes.resize(k + node->mNumMeshes);
		// 处理位于当前节点的每个网格
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// 节点对象仅包含索引用来索引场景中的实际对象。
			// 场景包含所有数据，节点只是为了有组织的保存东西（如节点之间的关系）。
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			processMesh(mesh, scene, outMeshes[i + k]);
		}
		// 在我们处理完所有网格（如果有的话）后，我们会递归处理每个子节点
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene, outMeshes);
		}
	}

	void processMesh(aiMesh* mesh, const aiScene* scene, Mesh& outMesh)
	{
		outMesh.vertices.resize(mesh->mNumVertices);

		std::vector<Vertex>& vertices = outMesh.vertices;
		std::vector<unsigned int>& indices = outMesh.indices;

		for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
		{
			Vertex& vertex = vertices[i];
			// 位置
			vertex.Position.x = mesh->mVertices[i].x;
			vertex.Position.y = mesh->mVertices[i].y;
			vertex.Position.z = mesh->mVertices[i].z;
			// 法线
			vertex.Normal.x = mesh->mNormals[i].x;
			vertex.Normal.y = mesh->mNormals[i].y;
			vertex.Normal.z = mesh->mNormals[i].z;
			// 纹理坐标
			if (mesh->mTextureCoords[0]) // 网格是否包含纹理坐标？
			{
				// 顶点最多可包含8个不同的纹理坐标。 因此，我们假设我们不会使用顶点可以具有多个纹理坐标的模型，因此我们总是采用第一个集合（0）。
				vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
				vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
			}
			// u向量
			vertex.Tangent.x = mesh->mTangents[i].x;
			vertex.Tangent.y = mesh->mTangents[i].y;
			vertex.Tangent.z = mesh->mTangents[i].z;
			// v向量
			vertex.Bitangent.x = mesh->mBitangents[i].x;
			vertex.Bitangent.y = mesh->mBitangents[i].y;
			vertex.Bitangent.z = mesh->mBitangents[i].z;
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// 检索面的所有索引并将它们存储在索引向量中
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		Mesh mm;
	}

}
