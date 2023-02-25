#include <fstream>
#include <yaml-cpp/yaml.h>
#include "EngineRuntime/include/Resource/AssetManager/AssetManager.h"
#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Function/Framework/Object/Actor.h"
#include "EngineRuntime/include/Platform/FileSystem/FileSystem.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/AnimationClip.h"

namespace Engine
{
	bool AssetManager::Initialize(const std::filesystem::path& projectPath)
	{
		path assetsDir = ProjectFileSystem::GetInstance()->GetActualPath(projectPath) / "Assets";

		dfsFolder(assetsDir, ProjectFileSystem::GetInstance()->GetActualPath(projectPath));

		LoadDefaultMesh();

		return true;
	}

	void AssetManager::Finalize()
	{
		for (auto asset : mCacheAsset)
		{
			if (asset.second != nullptr)
			{
				delete asset.second;
			}
		}
	}

	void AssetManager::AddAsset(const std::filesystem::path& assetPath, const GUID& guid)
	{
		ASSERT(mFilesGUID.find(assetPath) == mFilesGUID.end());

		mFilesGUID[assetPath] = guid;

		mFilesPath[guid] = assetPath;

		LOG_INFO("添加资产, 路径: {0} ,GUID: {1}", assetPath.generic_string().c_str(), guid.Data());
	}

	void AssetManager::RemoveAsset(const std::filesystem::path& assetPath)
	{
		ASSERT(mFilesGUID.find(assetPath) != mFilesGUID.end());

		mFilesPath.erase(mFilesGUID[assetPath]);
		mFilesGUID.erase(assetPath);

		LOG_INFO("移除资产, 路径: {0}", assetPath.generic_string().c_str());
	}

	void AssetManager::MoveAsset(const std::filesystem::path& oldPath, const std::filesystem::path& newPath)
	{
		ASSERT(mFilesGUID.find(oldPath) != mFilesGUID.end());
		ASSERT(mFilesGUID.find(newPath) == mFilesGUID.end());
		GUID guid = mFilesGUID[oldPath];
		mFilesGUID.erase(oldPath);
		mFilesGUID[newPath] = guid;
		mFilesPath[guid] = newPath;
		LOG_INFO("移动资产, 从 {0} 移动到 {1}", oldPath.generic_string().c_str(), newPath.generic_string().c_str());
	}

	GUID AssetManager::GetAssetGuidFormAssetPath(const std::filesystem::path& assetPath)
	{
		auto it = mFilesGUID.find(assetPath);
		if (it == mFilesGUID.end())
		{
			return GUID();
		}

		return it->second;
	}

	std::filesystem::path AssetManager::GetAssetPathFormAssetGuid(const GUID& guid)
	{
		if (!guid.IsVaild())
		{
			LOG_ERROR("Guid 无效");
			return path();
		}

		auto it = mFilesPath.find(guid);

		if (it != mFilesPath.end())
		{
			return it->second;
		}

		LOG_ERROR("Guid {0} 没有找到对应路径", guid.Data());

		return std::filesystem::path();
	}

	GUID AssetManager::GetDefaultAssetGuid(DefaultMesh _mesh)
	{
		switch (_mesh)
		{
		case DefaultMesh_Box:
			return mDefaultBoxMeshGuid;
			break;
		case DefaultMesh_Cylinder:
			return mDefaultCylinderMeshGuid;
			break;
		case DefaultMesh_Grid:
			return mDefaultGridMeshGuid;
			break;
		case DefaultMesh_Quad:
			return mDefaultQuadMeshGuid;
			break;
		case DefaultMesh_Sphere:
			return mDefaultSphereMeshGuid;
			break;
		case DefaultMesh_ScreenQuad:
			return mDefaultScreenQuadMeshGuid;
			break;
		default:
			break;
		}

		return GUID();
	}

	Object* AssetManager::LoadResource(const GUID& guid)
	{
		auto it = mCacheAsset.find(guid);

		if (it == mCacheAsset.end())
		{
			auto it = mFilesPath.find(guid);

			if (it == mFilesPath.end())
			{
				return nullptr;
			}
			
			return LoadResource(it->second);
		}

		return it->second;

	}

	Object* AssetManager::LoadResource(const std::filesystem::path& resourcePath)
	{
		std::string ext = resourcePath.extension().generic_string();
		if (ext == ".texture")
		{
			return LoadResourceFromFile<TextureData>(resourcePath);
		}
		else if (ext == ".material")
		{
			return LoadResourceFromFile<MaterialData>(resourcePath);
		}
		else if (ext == ".mesh")
		{
			return LoadResourceFromFile<Mesh>(resourcePath);
		}
		else if (ext == ".prefab")
		{
			return LoadResourceFromFile<Actor>(resourcePath);
		}
		else if (ext == ".skeleton")
		{
			return LoadResourceFromFile<SkeletonData>(resourcePath);
		}
		else if (ext == ".animation")
		{
			return LoadResourceFromFile<AnimationClip>(resourcePath);
		}
		else
		{
			ASSERT(0);
		}

		return nullptr;
	}

	Mesh* AssetManager::LoadDefaultMeshResource(DefaultMesh _mesh)
	{
		switch (_mesh)
		{
		case DefaultMesh_Box:
			return LoadResource<Mesh>(mDefaultBoxMeshGuid);
			break;
		case DefaultMesh_Cylinder:
			return LoadResource<Mesh>(mDefaultCylinderMeshGuid);
			break;
		case DefaultMesh_Grid:
			return LoadResource<Mesh>(mDefaultGridMeshGuid);
			break;
		case DefaultMesh_Quad:
			return LoadResource<Mesh>(mDefaultQuadMeshGuid);
			break;
		case DefaultMesh_ScreenQuad:
			return LoadResource<Mesh>(mDefaultScreenQuadMeshGuid);
			break;
		case DefaultMesh_Sphere:	
			return LoadResource<Mesh>(mDefaultSphereMeshGuid);
			break;
		}

		return nullptr;
	}

	void AssetManager::UnLoadResource(const Object* resource)
	{
		// TODO: 资源卸载管理
	}

	void AssetManager::dfsFolder(const std::filesystem::path& currentPath, const std::filesystem::path& projectPath)
	{
		for (const auto& file : std::filesystem::directory_iterator(currentPath))
		{
			if (file.is_directory())
			{
				dfsFolder(file, projectPath);
				continue;
			}
			auto filePath = file.path();
			if (filePath.extension() == ".meta")
			{
				continue;
			}

			auto fileMetaPath = file.path();
			fileMetaPath += ".meta";

			ASSERT(ProjectFileSystem::GetInstance()->FileExists(fileMetaPath));
			
			YAML::Node node = YAML::LoadFile(fileMetaPath.generic_string());

			filePath = filePath.lexically_relative(projectPath);

			ASSERT(node["guid"].IsDefined());
			ASSERT(mFilesGUID.find(filePath) == mFilesGUID.end());

			mFilesGUID[filePath] = node["guid"].as<std::string>();
			mFilesPath[mFilesGUID[filePath]] = filePath;
		}

	}

	void AssetManager::LoadDefaultMesh()
	{
		{
			Mesh* boxMesh = new Mesh;
			Mesh::CreateBox(*boxMesh, 1.0f, 1.0f, 1.0f, 3);
			mDefaultBoxMeshGuid = GUID("9D4917EB9694472B99B2793E3668B9AC");
			ASSERT(boxMesh->Meshes.size() == 1);
			boxMesh->Meshes[0].mGuid = mDefaultBoxMeshGuid;
			mCacheAsset.emplace(mDefaultBoxMeshGuid, boxMesh);
		}

		{
			Mesh* cylinderMesh = new Mesh;
			Mesh::CreateCylinder(*cylinderMesh, 0.5f, 0.3f, 3.0f, 20, 20);
			mDefaultCylinderMeshGuid = GUID("6BCE17D01FD04276982BBF90698E1D03");
			ASSERT(cylinderMesh->Meshes.size() == 1);
			cylinderMesh->Meshes[0].mGuid = mDefaultCylinderMeshGuid;
			mCacheAsset.emplace(mDefaultCylinderMeshGuid, cylinderMesh);
		}

		{
			Mesh* gridMesh = new Mesh;
			Mesh::CreateGrid(*gridMesh, 20.0f, 30.0f, 60, 40);
			mDefaultGridMeshGuid = GUID("C59915B55127450CA4629899E8CB22D5");
			ASSERT(gridMesh->Meshes.size() == 1);
			gridMesh->Meshes[0].mGuid = mDefaultGridMeshGuid;
			mCacheAsset.emplace(mDefaultGridMeshGuid, gridMesh);
		}

		{
			Mesh* quadMesh = new Mesh;
			Mesh::CreateQuad(*quadMesh, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f);
			mDefaultQuadMeshGuid = GUID("BF54081E79C74E419AF036C2AE8415C9");
			ASSERT(quadMesh->Meshes.size() == 1);
			quadMesh->Meshes[0].mGuid = mDefaultQuadMeshGuid;
			mCacheAsset.emplace(mDefaultQuadMeshGuid, quadMesh);
		}

		{
			Mesh* sphereMesh = new Mesh;
			Mesh::CreateSphere(*sphereMesh, 0.5f, 20, 20);
			mDefaultSphereMeshGuid = GUID("A00C0EF279F14F39A32E4CC86FCFE6DB");
			ASSERT(sphereMesh->Meshes.size() == 1);
			sphereMesh->Meshes[0].mGuid = mDefaultSphereMeshGuid;
			mCacheAsset.emplace(mDefaultSphereMeshGuid, sphereMesh);
		}

		{
			Mesh* screenQuadMesh = new Mesh;
			Mesh::CreateQuad(*screenQuadMesh, -1.0f, 1.0f, 2.0f, 2.0f, 0.0f);
			mDefaultScreenQuadMeshGuid = GUID("2CBE0D51BBE448A9967948534786C112");
			ASSERT(screenQuadMesh->Meshes.size() == 1);
			screenQuadMesh->Meshes[0].mGuid = mDefaultScreenQuadMeshGuid;
			mCacheAsset.emplace(mDefaultScreenQuadMeshGuid, screenQuadMesh);

		}
	}

	Object* AssetManager::FindCache(const std::filesystem::path& resourcePath)
	{
		auto itF = mFilesGUID.find(resourcePath);

		ASSERT(itF != mFilesGUID.end());

		auto itG = mCacheAsset.find(itF->second);

		if (itG == mCacheAsset.end())
		{
			return nullptr;
		}

		return itG->second;
	}

}
