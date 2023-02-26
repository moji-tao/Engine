#include <assimp/scene.h>
#include <assimp/Importer.hpp> 
#include <assimp/postprocess.h>
#include <stb/stb_image.h>
#include "EngineEditor/include/EditorTools/Loader/MeshLoader.h"
#include "EngineEditor/include/EditorTools/Loader/TextureLoader.h"
#include "EngineEditor/include/Application/EngineEditor.h"
#include "EngineRuntime/include/Function/Framework/Component/MeshRendererComponent.h"
#include "EngineRuntime/include/Function/Framework/Component/SkeletonMeshRendererComponent.h"
#include "EngineRuntime/include/Function/Framework/Object/Actor.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/AnimationClip.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/MaterialData.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/MeshData.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/SkeletonData.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/TextureData.h"

namespace Editor
{
	using namespace Engine;

	void LoadMesh(const aiScene* scene, Engine::Mesh& mesh, const SkeletonData* skeleton);

	//void LoadAnimations()

	void LoadSkeleton(aiNode* node, Engine::Joint* joint, uint32_t& jointNum, SkeletonData& skeleton);

	void LoadMeshActor(aiNode* node, Engine::Actor* actorNode);

	void LoadSkeletonActor(aiNode* node, Engine::Actor* actorNode);

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
			aiProcess_Triangulate | aiProcess_GenBoundingBoxes | aiProcess_ConvertToLeftHanded |
			aiProcess_SortByPType | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights | aiProcess_PopulateArmatureData);
		/*
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
		{
			LOG_ERROR("错误::ASSIMP:: {0}", importer.GetErrorString());
			return Engine::GUID();
		}
		*/
		if (scene == nullptr)
		{
			LOG_ERROR("错误::ASSIMP:: {0}", importer.GetErrorString());
			return Engine::GUID();
		}
		Mesh mesh;
		Actor actor;
		Joint rootJoint;
		SkeletonData skeletonData;

		AssetsFileSystem* fileSystem = EngineEditor::GetInstance()->GetFileSystem();

		std::vector<Engine::GUID> mRefTexture;

		if (scene->HasTextures())
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
			for (uint32_t i = 0; i < scene->mNumAnimations; ++i)
			{
				aiAnimation& animation = *scene->mAnimations[i];
				AnimationClip animationClip;
				if (scene->mNumAnimations == 1)
				{
					animationClip.SetAnimationClipName(fileName);
				}
				else
				{
					animationClip.SetAnimationClipName(animation.mName.C_Str());
				}

				LOG_INFO("动画时长: {0}  TicksPerSecond: {1}", animation.mDuration, animation.mTicksPerSecond);
				animationClip.SetAnimationTime(animation.mDuration);
				animationClip.SetTicksPerSecond(animation.mTicksPerSecond);

				for (uint32_t j = 0; j < animation.mNumChannels; ++j)
				{
					LOG_INFO("骨骼: {0}", animation.mChannels[j]->mNodeName.C_Str());

					ASSERT(animation.mChannels[j]->mNumPositionKeys == animation.mChannels[j]->mNumScalingKeys &&
						animation.mChannels[j]->mNumPositionKeys == animation.mChannels[j]->mNumRotationKeys);

					for (uint32_t i = 0; i < animation.mChannels[j]->mNumPositionKeys; ++i)
					{
						ASSERT(animation.mChannels[j]->mPositionKeys[i].mTime == animation.mChannels[j]->mScalingKeys[i].mTime &&
							animation.mChannels[j]->mScalingKeys[i].mTime == animation.mChannels[j]->mRotationKeys[i].mTime);

						double time = animation.mChannels[j]->mPositionKeys[i].mTime;

						Transform transform;
						transform.position = Vector3(animation.mChannels[j]->mPositionKeys[i].mValue.x,
							animation.mChannels[j]->mPositionKeys[i].mValue.y,
							animation.mChannels[j]->mPositionKeys[i].mValue.z);
						transform.scale = Vector3(animation.mChannels[j]->mScalingKeys[i].mValue.x,
							animation.mChannels[j]->mScalingKeys[i].mValue.y,
							animation.mChannels[j]->mScalingKeys[i].mValue.z);
						transform.rotation = Quaternion(animation.mChannels[j]->mRotationKeys[i].mValue.w,
							animation.mChannels[j]->mRotationKeys[i].mValue.x,
							animation.mChannels[j]->mRotationKeys[i].mValue.y,
							animation.mChannels[j]->mRotationKeys[i].mValue.z);

						animationClip.AddBoneKey(animation.mChannels[j]->mNodeName.C_Str(), time, transform);
					}

					/*
					if (animation.mChannels[j]->mNumPositionKeys != 0)
					{
						for (uint32_t k = 0; k < animation.mChannels[j]->mNumPositionKeys; ++k)
						{
							LOG_INFO("\tTime: {0}, Position: ({1}, {2}, {3})", animation.mChannels[j]->mPositionKeys[k].mTime,
								animation.mChannels[j]->mPositionKeys[k].mValue.x, 
								animation.mChannels[j]->mPositionKeys[k].mValue.y, 
								animation.mChannels[j]->mPositionKeys[k].mValue.z);
						}
					}
					if (animation.mChannels[j]->mNumRotationKeys != 0)
					{
						for (uint32_t k = 0; k < animation.mChannels[j]->mNumRotationKeys; ++k)
						{
							LOG_INFO("\tTime: {0}, Rotation: ({1}, {2}, {3}, {4})", animation.mChannels[j]->mRotationKeys[k].mTime,
								animation.mChannels[j]->mRotationKeys[k].mValue.x,
								animation.mChannels[j]->mRotationKeys[k].mValue.y,
								animation.mChannels[j]->mRotationKeys[k].mValue.z,
								animation.mChannels[j]->mRotationKeys[k].mValue.w);
						}
					}

					if (animation.mChannels[j]->mNumScalingKeys != 0)
					{
						for (uint32_t k = 0; k < animation.mChannels[j]->mNumScalingKeys; ++k)
						{
							LOG_INFO("\tTime: {0}, Scaling: ({1}, {2}, {3})", animation.mChannels[j]->mScalingKeys[k].mTime,
								animation.mChannels[j]->mScalingKeys[k].mValue.x,
								animation.mChannels[j]->mScalingKeys[k].mValue.y,
								animation.mChannels[j]->mScalingKeys[k].mValue.z);
						}
					}
					*/
				}

				SerializerDataFrame frame;
				frame << animationClip;
				AnimationClipMeta meta;
				meta.SetGuid(Engine::GUID::Get());
				
				fileSystem->CreateAssetFile(dstFolder, animationClip.GetAnimationClipName() + ".animation", frame, meta);
				LOG_INFO("导入动画 {0}, 保存路径 {1}", animation.mName.C_Str(), dstFolder->GetAssetFilePath().generic_string().c_str());

			}
		}

		bool hasSkinned = false;

		if (scene->mRootNode != nullptr)
		{
			aiNode* skinned = scene->mRootNode->FindNode("Armature");
			aiNode* body = scene->mRootNode->FindNode("Body");

			if (skinned != nullptr)
			{
				// 加载骨骼
				skeletonData.AddRootJoint(&rootJoint);
				uint32_t jointNum = 0;
				LoadSkeleton(skinned, &rootJoint, jointNum, skeletonData);
				
				hasSkinned = true;
			}

			if (body != nullptr)
			{
				actor.SetActorName(fileName);
				actor.AddComponent<TransformComponent>();

				if (skinned == nullptr)
				{
					LoadMeshActor(body, &actor);
				}
				else
				{
					LoadSkeletonActor(body, &actor);
				}

				SerializerDataFrame frame;
				frame << actor;
				PrefabMeta meta;
				meta.SetGuid(Engine::GUID::Get());

				fileSystem->CreateAssetFile(dstFolder, (fileName + ".prefab"), frame, meta);
				LOG_INFO("导入预制体 {0}, 保存路径 {1}", fileName.c_str(), dstFolder->GetAssetFilePath().generic_string().c_str());
			}
		}

		if (scene->HasMeshes())
		{
			if (hasSkinned)
			{
				LoadMesh(scene, mesh, &skeletonData);
			}
			else
			{
				LoadMesh(scene, mesh, nullptr);
			}
			SerializerDataFrame frame;
			frame << mesh;
			MeshMeta meta;
			meta.SetGuid(mesh.GetGuid());

			fileSystem->CreateAssetFile(dstFolder, fileName + ".mesh", frame, meta);
			LOG_INFO("导入模型 {0}, 保存路径 {1}", fileName.c_str(), dstFolder->GetAssetFilePath().generic_string().c_str());
		}

		if (hasSkinned)
		{
			SerializerDataFrame frame;
			frame << skeletonData;
			SkeletonMeta meta;
			meta.SetGuid(Engine::GUID::Get());
			fileSystem->CreateAssetFile(dstFolder, (fileName + ".skeleton"), frame, meta);
			LOG_INFO("导入骨骼 {0}, 保存路径 {1}", fileName.c_str(), dstFolder->GetAssetFilePath().generic_string().c_str());
		}

		importer.FreeScene();

		return mesh.GetGuid();
	}

	void LoadMesh(const aiScene* scene, Engine::Mesh& mesh, const SkeletonData* skeleton)
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

			for (int j = 0; j < m_mesh.mNumBones; ++j)
			{
				if (skeleton != nullptr)
				{
					Joint* joint = skeleton->GetJoint(m_mesh.mBones[j]->mName.C_Str());
					if (joint == nullptr)
					{
						LOG_WARN("没有找到骨骼 {0}", m_mesh.mBones[j]->mName.C_Str());
					}
					else
					{
						memcpy(&joint->mModleToLocalMat, &m_mesh.mBones[j]->mOffsetMatrix, sizeof(joint->mModleToLocalMat));
						joint->mModleToLocalMat = joint->mModleToLocalMat.Transpose();

						for (int k = 0; k < m_mesh.mBones[j]->mNumWeights; ++k)
						{
							Vertex& vertex = subMesh.Vertices[m_mesh.mBones[j]->mWeights[k].mVertexId];
							vertex.BoneIndices[vertex.BoneNum] = joint->mJointNum;
							vertex.BoneWeights[vertex.BoneNum] = m_mesh.mBones[j]->mWeights[k].mWeight;
							++vertex.BoneNum;
						}
					}
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

	void LoadMeshActor(aiNode* node, Engine::Actor* actorNode)
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
			LOG_INFO("该节点 {0} 有模型数据 添加MeshRendererComponent", node->mName.C_Str());
			actorNode->AddComponent<Engine::MeshRendererComponent>();
		}

		for (unsigned i = 0; i < childrenNum; ++i)
		{
			Actor* childrenActor = new Actor();
			childrenActor->SetActorName(node->mChildren[i]->mName.C_Str());
			actorNode->AddChildrenForTools(childrenActor);
			childrenActor->AddComponent<TransformComponent>();
			LoadMeshActor(node->mChildren[i], childrenActor);
		}
	}

	void LoadSkeletonActor(aiNode* node, Engine::Actor* actorNode)
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
			LOG_INFO("该节点 {0} 有模型数据 添加SkeletonMeshRendererComponent", node->mName.C_Str());
			actorNode->AddComponent<Engine::SkeletonMeshRendererComponent>();
		}

		for (unsigned i = 0; i < childrenNum; ++i)
		{
			Actor* childrenActor = new Actor();
			childrenActor->SetActorName(node->mChildren[i]->mName.C_Str());
			actorNode->AddChildrenForTools(childrenActor);
			childrenActor->AddComponent<TransformComponent>();
			LoadSkeletonActor(node->mChildren[i], childrenActor);
		}
	}

	void LoadSkeleton(aiNode* node, Engine::Joint* joint, uint32_t& jointNum, SkeletonData& skeleton)
	{
		joint->mJointName = node->mName.C_Str();

		unsigned childrenNum = node->mNumChildren;
		joint->mChildrens.resize(childrenNum);

		Matrix4x4 m;
		memcpy(&m, &node->mTransformation, sizeof(m));
		m = m.Transpose();
		Vector3 position;
		Vector3 scale;
		Quaternion rotation;
		m.Decomposition(position, scale, rotation);
		joint->mTransform.position = position;
		joint->mTransform.scale = scale;
		joint->mTransform.rotation = rotation;
		joint->mJointNum = jointNum++;

		skeleton.AddJoint(joint->mJointName, joint);

		for (unsigned i = 0; i < childrenNum; ++i)
		{
			joint->mChildrens[i] = new Joint;
			LoadSkeleton(node->mChildren[i], joint->mChildrens[i], jointNum, skeleton);
			joint->mChildrens[i]->mParent = joint;
		}
	}
}
