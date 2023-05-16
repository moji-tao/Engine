#pragma once
#include <memory>
#include <filesystem>
#include "EngineRuntime/include/Core/Meta/Object.h"
#include "EngineRuntime/include/Core/Random/Random.h"
#include "EngineRuntime/include/Framework/Interface/ISingleton.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/MaterialData.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/SkeletonData.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/TextureData.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/MeshData.h"

namespace Engine
{
	enum DefaultMesh
	{
		DefaultMesh_Box,
		DefaultMesh_Cylinder,
		DefaultMesh_Grid,
		DefaultMesh_Quad,
		DefaultMesh_ScreenQuad,
		DefaultMesh_Sphere,
	};

	class AssetManager : public ISingleton<AssetManager>
	{
	public:
		bool Initialize(const std::filesystem::path& projectPath);

		void Finalize();

	public:
		void AddAsset(const std::filesystem::path& assetPath, const GUID& guid);

		void RemoveAsset(const std::filesystem::path& assetPath);

		void MoveAsset(const std::filesystem::path& oldPath, const std::filesystem::path& newPath);

		GUID GetAssetGuidFormAssetPath(const std::filesystem::path& assetPath);

		std::filesystem::path GetAssetPathFormAssetGuid(const GUID& guid);

		GUID GetDefaultAssetGuid(DefaultMesh _mesh);

	public:
		template<typename T>
		T* LoadResource(const std::filesystem::path& resourcePath)
		{
			Object* object = FindCache(resourcePath);

			if (object == nullptr)
			{
				return LoadResourceFromFile<T>(resourcePath);
			}

			return DynamicCast<T>(object);
		}

		template<typename T>
		T* LoadResource(const GUID& resourceGuid)
		{
			auto it = mCacheAsset.find(resourceGuid);

			if (it == mCacheAsset.end())
			{
				auto it = mFilesPath.find(resourceGuid);

				if (it == mFilesPath.end())
				{
					return nullptr;
				}

				return LoadResourceFromFile<T>(it->second);
			}

			return DynamicCast<T>(it->second);
		}

		Mesh* LoadDefaultMeshResource(DefaultMesh _mesh);

		template<typename T>
		T* LoadResource(const void* data, size_t length)
		{
			return LoadResourceFromMemory<T>(data, length);
		}

		Object* LoadResource(const GUID& guid);

		Object* LoadResource(const std::filesystem::path& resourcePath);

		void UnLoadResource(const Object* resource);

	private:
		void dfsFolder(const std::filesystem::path& currentPath, const std::filesystem::path& projectPath);

		void LoadDefaultMesh();

		Object* FindCache(const std::filesystem::path& resourcePath);

	private:
		template<typename T>
			requires std::is_class<Object>::value
		T* LoadResourceFromFile(const std::filesystem::path& resourcePath)
		{
			SerializerDataFrame frame;
			frame.Load(resourcePath);

			if (!ProjectFileSystem::GetInstance()->FileExists(resourcePath))
			{
				return nullptr;
			}

			T* result = new T();
			frame >> *result;

			auto itF = mFilesGUID.find(resourcePath);
			ASSERT(itF != mFilesGUID.end());
			mCacheAsset.emplace(itF->second, result);

			result->SetGuid(itF->second);

			return result;
		}

		template<typename T>
			requires std::is_class<Object>::value
		T* LoadResourceFromMemory(const void* data, size_t length)
		{
			SerializerDataFrame frame;
			frame.Load(data, length);

			T* result = new T();
			frame >> *result;

			return result;
		}
	private:
		std::unordered_map<GUID, Object*> mCacheAsset;

		std::unordered_map<std::filesystem::path, GUID> mFilesGUID;

		std::unordered_map<GUID, std::filesystem::path> mFilesPath;

		//std::unordered_map<>

	private:
		GUID mDefaultBoxMeshGuid;
		GUID mDefaultCylinderMeshGuid;
		GUID mDefaultGridMeshGuid;
		GUID mDefaultQuadMeshGuid;
		GUID mDefaultSphereMeshGuid;
		GUID mDefaultScreenQuadMeshGuid;
	};


}
