#include <fstream>
#include "EngineRuntime/include/Platform/FileSystem/FileSystem.h"
#include "EngineRuntime/include/Core/Base/macro.h"

namespace Engine
{
	Blob::Blob(void* data, size_t size) : mData(data), mSize(size) { }

	Blob::~Blob()
	{
		if(mData != nullptr)
		{
			free(mData);
		}
		mData = nullptr;
		mSize = 0;
	}

	const void* Blob::GetData() const
	{
		return mData;
	}

	size_t Blob::GetSize() const
	{
		return mSize;
	}

	bool NativeFileSystem::Initialize(InitConfig* info)
	{
		EngineFileSystem::GetInstance()->Initialize(info->enginePath);

		ProjectFileSystem::GetInstance()->Initialize(info->workspacePath);

		return true;
	}

	void NativeFileSystem::Finalize()
	{

	}

	std::shared_ptr<Blob> NativeFileSystem::ReadFile(const path& filePath)
	{
		if (!FileExists(filePath))
			return nullptr;

		std::ifstream file(filePath, std::ios::binary | std::ios::in);

		file.seekg(0, std::ios::end);
		uint64_t size = file.tellg();
		file.seekg(0, std::ios::beg);

		char* buffer = new char[size];
		file.read(buffer, size);

		if (!file.good() && !file.eof())
		{
			delete[] buffer;
			return nullptr;
		}

		return std::make_shared<Blob>(buffer, size);
	}

	bool NativeFileSystem::WriteFile(const path& filePath, const void* data, size_t size)
	{
		path parentDir = filePath.parent_path();
		if(!exists(parentDir))
		{
			if (create_directories(parentDir))
			{
				LOG_INFO("创建目录 {0}", parentDir.generic_string().c_str());
			}
			else
			{
				LOG_ERROR("创建目录失败 {0}", parentDir.generic_string().c_str());
			}
		}

		std::ofstream file(filePath, std::ios::binary | std::ios::out);
		if (!file.is_open())
		{
			return false;
		}

		if (size > 0)
		{
			file.write(static_cast<const char*>(data), size);
		}

		if (!file.good())
		{
			return false;
		}
		file.close();

		return true;
	}

	bool NativeFileSystem::WriteFile(const path& filePath, const std::string& data)
	{
		path parentDir = filePath.parent_path();
		if (!exists(parentDir))
		{
			if (create_directories(parentDir))
			{
				LOG_INFO("创建目录 {0}", parentDir.generic_string().c_str());
			}
			else
			{
				LOG_ERROR("创建目录失败 {0}", parentDir.generic_string().c_str());
			}
		}

		std::ofstream file(filePath, std::ios::binary | std::ios::out);
		if (!file.is_open())
		{
			return false;
		}

		if (data.size() > 0)
		{
			file << data;
		}

		if (!file.good())
		{
			return false;
		}
		file.close();

		return true;
	}

	bool NativeFileSystem::FolderExists(const path& folderPath)
	{
		return std::filesystem::exists(folderPath) && std::filesystem::is_directory(folderPath);
	}

	bool NativeFileSystem::FileExists(const path& filePath)
	{
		return std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath);
	}

	uintmax_t NativeFileSystem::GetFileSize(const path& filePath)
	{
		if (!std::filesystem::exists(filePath))
		{
			return 0;
		}
		return file_size(filePath);
	}

	void EngineFileSystem::Initialize(const std::filesystem::path& ph)
	{
		mRootPath = ph;
	}

	std::shared_ptr<Blob> EngineFileSystem::ReadFile(const path& filePath)
	{
		if (!FileExists(filePath))
			return nullptr;

		return NativeFileSystem::GetInstance()->ReadFile(mRootPath / filePath);
	}

	bool EngineFileSystem::WriteFile(const path& filePath, const void* data, size_t size)
	{
		return NativeFileSystem::GetInstance()->WriteFile(mRootPath / filePath, data, size);
	}

	bool EngineFileSystem::WriteFile(const path& filePath, const std::string& data)
	{
		return NativeFileSystem::GetInstance()->WriteFile(mRootPath / filePath, data);
	}

	bool EngineFileSystem::FolderExists(const path& folderPath)
	{
		return NativeFileSystem::GetInstance()->FolderExists(mRootPath / folderPath);
	}

	bool EngineFileSystem::FileExists(const path& filePath)
	{
		return NativeFileSystem::GetInstance()->FileExists(mRootPath / filePath);
	}

	path EngineFileSystem::GetActualPath(const path& filePath)
	{
		return mRootPath / filePath;
	}

	uintmax_t EngineFileSystem::GetFileSize(const path& filePath)
	{
		return NativeFileSystem::GetInstance()->GetFileSize(mRootPath / filePath);
	}

	void ProjectFileSystem::Initialize(const std::filesystem::path& ph)
	{
		mRootPath = ph;
	}

	std::shared_ptr<Blob> ProjectFileSystem::ReadFile(const path& filePath)
	{
		if (!FileExists(filePath))
			return nullptr;

		return NativeFileSystem::GetInstance()->ReadFile(mRootPath / filePath);
	}

	bool ProjectFileSystem::WriteFile(const path& filePath, const void* data, size_t size)
	{
		return NativeFileSystem::GetInstance()->WriteFile(mRootPath / filePath, data, size);
	}

	bool ProjectFileSystem::WriteFile(const path& filePath, const std::string& data)
	{
		return NativeFileSystem::GetInstance()->WriteFile(mRootPath / filePath, data);
	}

	bool ProjectFileSystem::FolderExists(const path& folderPath)
	{
		return NativeFileSystem::GetInstance()->FolderExists(mRootPath / folderPath);
	}

	bool ProjectFileSystem::FileExists(const path& filePath)
	{
		return NativeFileSystem::GetInstance()->FileExists(mRootPath / filePath);
	}

	path ProjectFileSystem::GetActualPath(const path& filePath)
	{
		return mRootPath / filePath;
	}

	uintmax_t ProjectFileSystem::GetFileSize(const path& filePath)
	{
		return NativeFileSystem::GetInstance()->GetFileSize(mRootPath / filePath);
	}
}
