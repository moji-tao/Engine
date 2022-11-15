#pragma once
#include <filesystem>
#include "EngineRuntime/include/Framework/Interface/IRuntimeModule.h"
#include "EngineRuntime/include/Framework/Interface/ISingleton.h"

namespace Engine
{
	using namespace std::filesystem;

	class Blob
	{
	public:
		Blob(void* data, size_t size);

		~Blob();

	public:
		const void* GetData() const;

		size_t GetSize() const;

	private:
		void* mData;

		size_t mSize;
	};

	class NativeFileSystem : public ISingleton<NativeFileSystem>
	{
	public:
		bool Initialize(InitConfig* info);

		void Finalize();

	public:
		std::shared_ptr<Blob> ReadFile(const path& filePath);

		bool WriteFile(const path& filePath, const void* data, size_t size);

		bool FolderExists(const path& folderPath);

		bool FileExists(const path& filePath);

		uintmax_t GetFileSize(const path& filePath);
	};

	class EngineFileSystem : public ISingleton<EngineFileSystem>
	{
	public:
		void Initialize(const std::filesystem::path& ph);

		~EngineFileSystem() = default;

	public:
		std::shared_ptr<Blob> ReadFile(const path& filePath);

		bool WriteFile(const path& filePath, const void* data, size_t size);

		bool FolderExists(const path& folderPath);

		bool FileExists(const path& filePath);

		path GetActualPath(const path& filePath);

		uintmax_t GetFileSize(const path& filePath);

	private:
		path mRootPath;
	};

	class ProjectFileSystem : public ISingleton<ProjectFileSystem>
	{
	public:
		void Initialize(const std::filesystem::path& ph);

		~ProjectFileSystem() = default;

	public:
		std::shared_ptr<Blob> ReadFile(const path& filePath);

		bool WriteFile(const path& filePath, const void* data, size_t size);

		bool FolderExists(const path& folderPath);

		bool FileExists(const path& filePath);

		path GetActualPath(const path& filePath);

		uintmax_t GetFileSize(const path& filePath);

	private:
		path mRootPath;
	};
}
