#pragma once
#include <string>
#include <filesystem>
#include "EngineRuntime/include/Core/Meta/Serializer.h"
#include "EngineRuntime/include/Platform/FileSystem/FileSystem.h"

namespace Editor
{
	class AssetFile
	{
	public:
		bool mIsFolder = false;
		std::string mName;
		AssetFile* mParent = nullptr;
		std::list<AssetFile*> next;

	public:
		AssetFile() = default;

		~AssetFile();

	public:
		std::shared_ptr<Engine::Blob> GetBinary();

		AssetFile* FindFile(const std::string& fileName);

		std::filesystem::path GetAssetFilePath();

		bool GetAssetMetaFilePath(std::filesystem::path& result);

		std::filesystem::path GetParentPath();
	};

	class AssetsFileSystem final
	{
	public:
		AssetsFileSystem(const std::filesystem::path& projectPath);

		~AssetsFileSystem();

	public:
		AssetFile* GetRootFile();

		AssetFile* GetAssetFile(const std::filesystem::path& filePath);

	public:
		// 创建文件
		enum class CreateResult
		{
			emSuccess,
			emFileExists,
			emError,
		};

		CreateResult CreateFolder(AssetFile* root, const std::string& folderName);

		CreateResult CreateFolder(AssetFile* root, const std::string& folderName, AssetFile*& out);

		CreateResult CreateFolder(const std::filesystem::path& folderPath);

		CreateResult CreateFolder(const std::filesystem::path& folderPath, AssetFile*& out);

		CreateResult CreateAssetFile(AssetFile* root, const std::string& fileName, Engine::SerializerDataFrame& frame, Engine::MetaFrame& meta);

		CreateResult CreateAssetFile(AssetFile* root, const std::string& fileName, Engine::SerializerDataFrame& frame, Engine::MetaFrame& meta, AssetFile*& out);

		CreateResult CreateAssetFile(const std::filesystem::path& filePath, Engine::SerializerDataFrame& frame, Engine::MetaFrame& meta);

		CreateResult CreateAssetFile(const std::filesystem::path& filePath, Engine::SerializerDataFrame& frame, Engine::MetaFrame& meta, AssetFile*& out);

		CreateResult CreateTextFile(AssetFile* root, const std::string& fileName, const char* str, Engine::MetaFrame& meta);

		CreateResult CreateTextFile(AssetFile* root, const std::string& fileName, const char* str, Engine::MetaFrame& meta, AssetFile*& out);

		CreateResult CreateTextFile(const std::filesystem::path& filePath, const char* str, Engine::MetaFrame& meta);

		CreateResult CreateTextFile(const std::filesystem::path& filePath, const char* str, Engine::MetaFrame& meta, AssetFile*& out);

	public:
		// 删除文件
		enum class DeleteResult
		{
			emSuccess,
			emFileNotExists,
			emError,
		};
		DeleteResult DeleteAssetFile(AssetFile* root);

		DeleteResult DeleteAssetFile(const std::filesystem::path& filePath);

		DeleteResult DeleteFolder(AssetFile* root);

	public:
		// 移动文件
		enum class MoveResult
		{
			emSuccess,
			emFileNotExists,
			emTargetNotFolder,
			emError,
		};
		MoveResult AssetFileMove(AssetFile* oldFile, AssetFile* targetFolder);

		MoveResult AssetFileMove(const std::filesystem::path& oldFilePath, const std::filesystem::path& targetFolderPath);

	private:
		void PathRevise(AssetFile* file, const std::filesystem::path& temporaryPath, const std::filesystem::path& absoluteRootPath);

	public:
		// 重命名文件
		enum class RenameResult
		{
			emSuccess,
			emFileConflict,
			emError,
		};
		RenameResult AssetFileRename(AssetFile* file, std::string newName);

		RenameResult AssetFileRename(const std::filesystem::path& filePath, const std::string& newName);


	private:
		void dfsCreateFile(AssetFile* root, const std::filesystem::path& currentPath);

	private:
		AssetFile* mAssetRoot = nullptr;
	};
}
