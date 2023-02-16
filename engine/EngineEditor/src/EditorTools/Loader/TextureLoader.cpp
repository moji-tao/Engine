#include <stb/stb_image.h>
#include "EngineEditor/include/EditorTools/Loader/TextureLoader.h"
#include "EngineEditor/include/Application/EngineEditor.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/TextureData.h"

namespace Editor
{
	using namespace Engine;

	Engine::GUID TextureLoader::Load(const std::filesystem::path& src, AssetFile* dstFolder)
	{
		if (!NativeFileSystem::GetInstance()->FileExists(src))
		{
			return Engine::GUID();
		}

		std::shared_ptr<TextureData> texture_data = LoadTextureForFile(src, true);
		SerializerDataFrame frame;
		frame << *texture_data;
		TextureMeta meta;
		meta.SetGuid(Engine::GUID::Get());
		
		AssetsFileSystem* filesystem = EngineEditor::GetInstance()->GetFileSystem();

		if (filesystem->CreateAssetFile(dstFolder, src.filename().generic_string() + ".texture", frame, meta) != AssetsFileSystem::CreateResult::emSuccess)
		{
			return Engine::GUID();
		}

		return meta.GetGuid();
	}

	Engine::GUID TextureLoader::LoadHDR(const std::filesystem::path& src, AssetFile* dstFolder)
	{
		if (!NativeFileSystem::GetInstance()->FileExists(src))
		{
			return Engine::GUID();
		}

		std::shared_ptr<TextureData> texture_data = LoadHDRTextureForFile(src);
		SerializerDataFrame frame;
		frame << *texture_data;
		TextureMeta meta;
		meta.SetGuid(Engine::GUID::Get());

		AssetsFileSystem* filesystem = EngineEditor::GetInstance()->GetFileSystem();

		if (filesystem->CreateAssetFile(dstFolder, src.filename().generic_string() + ".texture", frame, meta) != AssetsFileSystem::CreateResult::emSuccess)
		{
			return Engine::GUID();
		}

		return meta.GetGuid();
	}


	Engine::GUID TextureLoader::LoadForMemory(const void* data, uint64_t size, const std::string& fileName, AssetFile* dstFolder)
	{
		std::shared_ptr<TextureData> texture_data = LoadTextureForMemory(data, size, false);
		SerializerDataFrame frame;
		frame << *texture_data;
		TextureMeta meta;
		meta.SetGuid(Engine::GUID::Get());

		AssetsFileSystem* filesystem = EngineEditor::GetInstance()->GetFileSystem();

		if (filesystem->CreateAssetFile(dstFolder, fileName, frame, meta) != AssetsFileSystem::CreateResult::emSuccess)
		{
			return Engine::GUID();
		}

		return meta.GetGuid();
	}
}
