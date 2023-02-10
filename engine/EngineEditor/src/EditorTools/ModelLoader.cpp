#include <assimp/Importer.hpp> 
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb/stb_image.h>
#include "EngineEditor/include/EditorTools/ModelLoader.h"
#include "EngineEditor/include/Application/EngineEditor.h"
#include "EngineRuntime/include/Platform/FileSystem/FileSystem.h"
#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Function/Framework/Component/TransformComponent.h"
#include "EngineRuntime/include/Core/Meta/Serializer.h"
#include "EngineRuntime/include/Core/Random/Random.h"
#include "EngineRuntime/include/Function/Framework/Component/MeshRendererComponent.h"
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
		const aiScene* scene = importer.ReadFileFromMemory(modelFile->GetData(), modelFile->GetSize(),
			aiProcess_Triangulate | aiProcess_GenBoundingBoxes | aiProcess_ConvertToLeftHanded | aiProcess_SortByPType);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // 如果不是0
		{
			LOG_ERROR("错误::ASSIMP:: {0}", importer.GetErrorString());
			return;
		}
		
		Mesh mesh;
		Actor actor;
		std::vector<MaterialData> materials;
		AssetsFileSystem* fileSystem = EngineEditor::GetInstance()->GetFileSystem();

		if (scene->HasMeshes())
		{
			std::filesystem::path outPath = outDir / (fileName + ".mesh");
			LoadMesh(scene, mesh);
			SerializerDataFrame frame;
			frame << mesh;
			MeshMeta meta;
			meta.SetGuid(mesh.GetGuid());

			fileSystem->CreateAssetFile(folder, fileName + ".mesh", frame, meta);
			LOG_INFO("导入模型 {0}, 保存路径 {1}", fileName.c_str(), outPath.generic_string().c_str());
		}

		if (scene->HasTextures())
		{
			LOG_INFO("有贴图数据");
			int textureCount = scene->mNumTextures;
			for (int i = 0; i < textureCount; ++i)
			{
				aiTexture* m_texture = scene->mTextures[i];
				std::shared_ptr<TextureData> texture_data;
				LOG_INFO("贴图名: {0} , 宽: {1}, 高: {2}", m_texture->mFilename.C_Str(), m_texture->mWidth, m_texture->mHeight);
				if (m_texture->mHeight == 0)
				{
					texture_data = LoadTextureForMemory(m_texture->pcData, m_texture->mWidth, false);
				}
				else
				{
					ASSERT(0);
				}

				if (texture_data != nullptr)
				{
					std::filesystem::path filePath = m_texture->mFilename.C_Str();
					filePath += ".texture";
					SerializerDataFrame frame;
					frame << *texture_data;
					TextureMeta meta;
					meta.SetGuid(Engine::GUID::Get());

					fileSystem->CreateAssetFile(outDir / filePath, frame, meta);
				}
			}
		}

		if (scene->HasMaterials())
		{
			LoadMaterial(scene, materials, outDir, folder);
			LOG_INFO("有材质数据");
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
			std::filesystem::path outPath = outDir / (fileName + ".prefab");
			actor.SetActorName(fileName);
			actor.AddComponent<TransformComponent>();
			LoadActor(scene->mRootNode, &actor, mesh, materials);
			SerializerDataFrame frame;
			frame << actor;
			PrefabMeta meta;
			meta.SetGuid(Engine::GUID::Get());

			fileSystem->CreateAssetFile(folder, (fileName + ".prefab"), frame, meta);
			LOG_INFO("导入预制体 {0}, 保存路径 {1}", fileName.c_str(), outPath.generic_string().c_str());
		}

		importer.FreeScene();
	}

	void ModelLoader::LoadMesh(const aiScene* scene, Engine::Mesh& mesh)
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
			subMesh.MaterialIndex = m_mesh.mMaterialIndex;
			subMesh.mGuid = Engine::GUID::Get();
		}
		mesh.SetGuid(Engine::GUID::Get());
	}

	void ModelLoader::LoadMaterial(const aiScene* scene, std::vector<MaterialData>& materials, const std::filesystem::path& outDir, AssetFile* folder)
	{
		AssetsFileSystem* fileSystem = EngineEditor::GetInstance()->GetFileSystem();
		materials.resize(scene->mNumMaterials);
		for (unsigned i = 0; i < scene->mNumMaterials; ++i)
		{
			MaterialData& materialData = materials[i];
			aiMaterial& material = *scene->mMaterials[i];
			LOG_INFO("材质名称: {0}", material.GetName().C_Str());
			LOG_INFO("===========================================");
			if (material.Get(AI_MATKEY_COLOR_DIFFUSE, materialData.DiffuseColor) == aiReturn::aiReturn_SUCCESS)
			{
				LOG_INFO("漫反射颜色: ({0}, {1}, {2})", materialData.DiffuseColor[0], materialData.DiffuseColor[1], materialData.DiffuseColor[2]);
			}
			else
			{
				LOG_INFO("该材质没有漫反射颜色");
			}
			if (material.Get(AI_MATKEY_COLOR_SPECULAR, materialData.SpecularColor) == aiReturn::aiReturn_SUCCESS)
			{
				LOG_INFO("高光颜色: ({0}, {1}, {2})", materialData.SpecularColor[0], materialData.SpecularColor[1], materialData.SpecularColor[2]);
			}
			else
			{
				LOG_INFO("该材质没有高光颜色");
			}
			if (material.Get(AI_MATKEY_COLOR_AMBIENT, materialData.AmbientColor) == aiReturn::aiReturn_SUCCESS)
			{
				LOG_INFO("环境颜色: ({0}, {1}, {2})", materialData.AmbientColor[0], materialData.AmbientColor[1], materialData.AmbientColor[2]);
			}
			else
			{
				LOG_INFO("该材质没有环境颜色");
			}
			if (material.Get(AI_MATKEY_COLOR_REFLECTIVE, materialData.ReflectiveColor) == aiReturn::aiReturn_SUCCESS)
			{
				LOG_INFO("反射颜色: ({0}, {1}, {2})", materialData.ReflectiveColor[0], materialData.ReflectiveColor[1], materialData.ReflectiveColor[2]);
			}
			else
			{
				LOG_INFO("该材质没有反射颜色");
			}
			if (material.Get(AI_MATKEY_COLOR_EMISSIVE, materialData.EmissiveColor) == aiReturn::aiReturn_SUCCESS)
			{
				LOG_INFO("自发光颜色: ({0}, {1}, {2})", materialData.EmissiveColor[0], materialData.EmissiveColor[1], materialData.EmissiveColor[2]);
			}
			else
			{
				LOG_INFO("该材质没有自发光颜色");
			}
			if (material.Get(AI_MATKEY_COLOR_TRANSPARENT, materialData.TransparentColor) == aiReturn::aiReturn_SUCCESS)
			{
				LOG_INFO("透明色: ({0}, {1}, {2})", materialData.TransparentColor[0], materialData.TransparentColor[1], materialData.TransparentColor[2]);
			}
			else
			{
				LOG_INFO("该材质没有透明色");
			}
			if (material.Get(AI_MATKEY_OPACITY, materialData.Opacity) == aiReturn::aiReturn_SUCCESS)
			{
				LOG_INFO("不透明度: {0}%", materialData.Opacity * 100);
			}
			else
			{
				LOG_INFO("该材质没有定义不透明度");
			}
			if (material.Get(AI_MATKEY_SHININESS, materialData.Shininess) == aiReturn::aiReturn_SUCCESS)
			{
				LOG_INFO("粗糙度: {0}%", materialData.Shininess);
			}
			else
			{
				LOG_INFO("该材质没有定义粗糙度");
			}
			aiString diffuseTexPath;
			if (material.Get(AI_MATKEY_TEXTURE_DIFFUSE(0), diffuseTexPath) == aiReturn::aiReturn_SUCCESS)
			{
				diffuseTexPath.Append(".texture");
				auto textureGuid = Engine::AssetManager::GetInstance()->GetAssetGuidFormAssetPath(outDir / diffuseTexPath.C_Str());
				if (textureGuid.IsVaild())
				{
					materialData.RefDiffuseTexture = textureGuid;
					LOG_INFO("漫反射纹理路径: {0}", diffuseTexPath.C_Str());
				}
				else
				{
					LOG_WARN("漫反射纹理路径: {0}, 没有在编辑器中", diffuseTexPath.C_Str());
				}
			}
			else
			{
				LOG_INFO("该材质没有定义漫反射纹理路径");
			}
			aiString specularTexPath;
			if (material.Get(AI_MATKEY_TEXTURE_SPECULAR(0), specularTexPath) == aiReturn::aiReturn_SUCCESS)
			{
				specularTexPath.Append(".texture");
				auto textureGuid = Engine::AssetManager::GetInstance()->GetAssetGuidFormAssetPath(outDir / specularTexPath.C_Str());
				if (textureGuid.IsVaild())
				{
					materialData.RefDiffuseTexture = textureGuid;
					LOG_INFO("高光纹理路径: {0}", specularTexPath.C_Str());
				}
				else
				{
					LOG_WARN("高光纹理路径: {0}, 没有在编辑器中", specularTexPath.C_Str());
				}

			}
			else
			{
				LOG_INFO("该材质没有定义高光纹理路径");
			}
			aiString normalTexPath;
			if (material.Get(AI_MATKEY_TEXTURE_NORMALS(0), normalTexPath) == aiReturn::aiReturn_SUCCESS)
			{
				specularTexPath.Append(".texture");
				auto textureGuid = Engine::AssetManager::GetInstance()->GetAssetGuidFormAssetPath(outDir / normalTexPath.C_Str());
				if (textureGuid.IsVaild())
				{
					materialData.RefNormalTexture = textureGuid;
					LOG_INFO("法线贴图路径: {0}", normalTexPath.C_Str());
				}
				else
				{
					LOG_WARN("法线贴图路径: {0}, 没有在编辑器中", normalTexPath.C_Str());
				}

			}
			else
			{
				LOG_INFO("该材质没有定义法线贴图路径");
			}
			aiString heightTexPath;
			if (material.Get(AI_MATKEY_TEXTURE_HEIGHT(0), heightTexPath) == aiReturn::aiReturn_SUCCESS)
			{
				specularTexPath.Append(".texture");
				auto textureGuid = Engine::AssetManager::GetInstance()->GetAssetGuidFormAssetPath(outDir / heightTexPath.C_Str());
				if (textureGuid.IsVaild())
				{
					materialData.RefHeightTexture = textureGuid;
					LOG_INFO("高度贴图路径: {0}", heightTexPath.C_Str());
				}
				else
				{
					LOG_WARN("高度贴图路径: {0}, 没有在编辑器中", heightTexPath.C_Str());
				}

			}
			else
			{
				LOG_INFO("该材质没有定义高度贴图路径");
			}

			LOG_INFO("===========================================");
			materialData.SetGuid(Engine::GUID::Get());
			SerializerDataFrame frame;
			frame << materialData;
			MaterialMeta meta;
			meta.SetGuid(materialData.GetGuid());
			
			fileSystem->CreateAssetFile(folder, std::string(material.GetName().C_Str()) + ".material", frame, meta);
		}
	}

	void ModelLoader::LoadActor(aiNode* node, Engine::Actor* actorNode, const Engine::Mesh& mesh, std::vector<Engine::MaterialData>& materials)
	{
		unsigned childrenNum = node->mNumChildren;
		Matrix4x4 m;
		memcpy(&m, &node->mTransformation, sizeof(m));
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

			for (int i = 0; i < node->mNumMeshes; ++i)
			{
				uint32_t materialIndex = mesh.Meshes[node->mMeshes[i]].MaterialIndex;
				renderComponent->AddRefMaterial();
				renderComponent->SetRefMaterial(i, materials[materialIndex].GetGuid());
				LOG_INFO("{0} 号SubMesh添加 {1} 号材质, 材质Guid: {2}", i, materialIndex, materials[materialIndex].GetGuid().Data());
			}
		}

		for (unsigned i = 0; i < childrenNum; ++i)
		{
			Actor* childrenActor = new Actor();
			childrenActor->SetActorName(node->mChildren[i]->mName.C_Str());
			actorNode->AddChildrenForTools(childrenActor);
			childrenActor->AddComponent<TransformComponent>();
			LoadActor(node->mChildren[i], childrenActor, mesh, materials);
		}
		
	}
}
