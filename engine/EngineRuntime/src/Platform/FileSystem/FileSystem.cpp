#include "EngineRuntime/include/Platform/FileSystem/FileSystem.h"

namespace Engine
{
	FileSystem::~FileSystem()
	{ }

	bool FileSystem::Initialize(InitConfig* info)
	{
		mRootPath[FileSystemType::EnginePath] = info->enginePath;

		mRootPath[FileSystemType::AssectPath] = info->workspacePath;

		return true;
	}

	bool FileSystem::Tick(float deltaTile)
	{
		return true;
	}

	void FileSystem::Finalize()
	{ }

	bool FileSystem::CreateDirectory(FileSystemType type, const path& filePath)
	{
		if (Exists(type, filePath))
			return false;

		return create_directories(mRootPath[type] / filePath);
	}

	path FileSystem::GetActualPath(FileSystemType type, const path& filePath)
	{
		if (Exists(type, filePath))
		{
			return mRootPath[type] / filePath;
		}

		return "";
	}

	bool FileSystem::Exists(FileSystemType type, const path& filePath)
	{
		return exists(mRootPath[type] / filePath);
	}

	uintmax_t FileSystem::GetFileSize(FileSystemType type, const path& filePath)
	{
		if (Exists(type, filePath))
			return 0;
		return file_size(mRootPath[type] / filePath);
	}


}
