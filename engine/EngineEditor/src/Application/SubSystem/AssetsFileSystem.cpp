#include "EngineEditor/include/Application/SubSystem/AssetsFileSystem.h"
#include "EngineRuntime/include/Resource/AssetManager/AssetManager.h"

namespace Editor
{
	AssetFile::~AssetFile()
	{
		if (mIsFolder)
		{
			for (AssetFile* file : next)
			{
				if (file == nullptr)
				{
					continue;
				}

				delete file;
			}
		}
	}

	std::shared_ptr<Engine::Blob> AssetFile::GetBinary()
	{
		std::filesystem::path result;
		AssetFile* node = this;
		while (node)
		{
			result = node->mName / result;
			node = node->mParent;
		}
		result = result.parent_path();

		return Engine::ProjectFileSystem::GetInstance()->ReadFile(result);
	}

	AssetFile* AssetFile::FindFile(const std::string& fileName)
	{
		for (AssetFile* file : next)
		{
			if (file->mName == fileName)
			{
				return file;
			}
		}
		return nullptr;
	}

	std::filesystem::path AssetFile::GetAssetFilePath()
	{
		return GetParentPath() / mName;
	}

	bool AssetFile::GetAssetMetaFilePath(std::filesystem::path& result)
	{
		if (mIsFolder)
		{
			return false;
		}
		result = GetParentPath() / (mName + ".meta");

		return true;
	}

	std::filesystem::path AssetFile::GetParentPath()
	{
		if (mParent == nullptr)
		{
			return std::filesystem::path();
		}

		return mParent->GetParentPath() / mParent->mName;
	}

	Engine::GUID AssetFile::GetAssetGuid()
	{
		return Engine::AssetManager::GetInstance()->GetAssetGuidFormAssetPath(GetAssetFilePath());
	}


	AssetsFileSystem::AssetsFileSystem(const std::filesystem::path& projectPath)
	{
		std::filesystem::path assetsPath = projectPath / "Assets";
		mAssetRoot = new AssetFile;
		mAssetRoot->mIsFolder = true;
		mAssetRoot->mParent = nullptr;
		mAssetRoot->mName = "Assets";
		dfsCreateFile(mAssetRoot, Engine::ProjectFileSystem::GetInstance()->GetActualPath(assetsPath));
	}

	AssetsFileSystem::~AssetsFileSystem()
	{
		if (mAssetRoot != nullptr)
		{
			delete mAssetRoot;
			mAssetRoot = nullptr;
		}
	}

	AssetFile* AssetsFileSystem::GetRootFile()
	{
		ASSERT(mAssetRoot != nullptr);

		return mAssetRoot;
	}

	AssetFile* AssetsFileSystem::GetAssetFile(const std::filesystem::path& filePath)
	{
		ASSERT(mAssetRoot);

		AssetFile* currentFile = mAssetRoot;
		auto it = filePath.begin();

		if (it->filename() != currentFile->mName)
		{
			return nullptr;
		}
		++it;

		for (; it != filePath.end(); ++it)
		{
			if (!currentFile->mIsFolder)
			{
				return nullptr;
			}

			currentFile = currentFile->FindFile(it->filename().generic_string());
		}

		return currentFile;
	}

	AssetsFileSystem::CreateResult AssetsFileSystem::CreateFolder(AssetFile* root, const std::string& folderName)
	{
		if (root == nullptr || !root->mIsFolder)
		{
			return CreateResult::emError;
		}

		if (root->FindFile(folderName) != nullptr)
		{
			return CreateResult::emFileExists;
		}

		AssetFile* file = new AssetFile;
		file->mIsFolder = true;
		file->mName = folderName;
		file->mParent = root;

		root->next.push_back(file);

		std::filesystem::create_directory(Engine::ProjectFileSystem::GetInstance()->GetActualPath(file->GetAssetFilePath()));

		return CreateResult::emSuccess;
	}

	AssetsFileSystem::CreateResult AssetsFileSystem::CreateFolder(AssetFile* root, const std::string& folderName, AssetFile*& out)
	{
		if (root == nullptr || !root->mIsFolder)
		{
			out = nullptr;
			return CreateResult::emError;
		}

		if (out = root->FindFile(folderName))
		{
			return CreateResult::emFileExists;
		}

		AssetFile* file = new AssetFile;
		file->mIsFolder = true;
		file->mName = folderName;
		file->mParent = root;

		root->next.push_back(file);

		out = file;

		std::filesystem::create_directory(Engine::ProjectFileSystem::GetInstance()->GetActualPath(file->GetAssetFilePath()));

		return CreateResult::emSuccess;
	}

	AssetsFileSystem::CreateResult AssetsFileSystem::CreateFolder(const std::filesystem::path& folderPath)
	{
		ASSERT(mAssetRoot);

		AssetFile* currentFile = mAssetRoot;
		auto it = folderPath.begin();

		if (it->filename() != currentFile->mName)
		{
			return CreateResult::emError;
		}
		++it;

		CreateResult result = CreateResult::emFileExists;

		for (; it != folderPath.end(); ++it)
		{
			std::string folderName = it->filename().generic_string();

			result = CreateFolder(currentFile, folderName, currentFile);

			if (result == CreateResult::emError)
			{
				return CreateResult::emError;
			}
		}

		return result;
	}

	AssetsFileSystem::CreateResult AssetsFileSystem::CreateFolder(const std::filesystem::path& folderPath, AssetFile*& out)
	{
		ASSERT(mAssetRoot);

		AssetFile* currentFile = mAssetRoot;
		auto it = folderPath.begin();

		if (it->filename() != currentFile->mName)
		{
			out = nullptr;
			return CreateResult::emError;
		}
		++it;

		CreateResult result = CreateResult::emFileExists;

		for (; it != folderPath.end(); ++it)
		{
			std::string folderName = it->filename().generic_string();

			result = CreateFolder(currentFile, folderName, currentFile);

			if (result == CreateResult::emError)
			{
				out = nullptr;
				return CreateResult::emError;
			}
		}

		out = currentFile;

		return result;
	}

	AssetsFileSystem::CreateResult AssetsFileSystem::CreateAssetFile(AssetFile* root, const std::string& fileName, Engine::SerializerDataFrame& frame, Engine::MetaFrame& meta)
	{
		if (root == nullptr || !root->mIsFolder)
		{
			return CreateResult::emError;
		}

		if (root->FindFile(fileName) != nullptr)
		{
			return CreateResult::emFileExists;
		}

		AssetFile* file = new AssetFile;
		file->mIsFolder = false;
		file->mName = fileName;
		file->mParent = root;

		root->next.push_back(file);

		frame.Save(file->GetAssetFilePath(), &meta);

		Engine::AssetManager::GetInstance()->AddAsset(file->GetAssetFilePath(), meta.GetGuid());

		return CreateResult::emSuccess;
	}

	AssetsFileSystem::CreateResult AssetsFileSystem::CreateAssetFile(AssetFile* root, const std::string& fileName, Engine::SerializerDataFrame& frame, Engine::MetaFrame& meta, AssetFile*& out)
	{
		if (root == nullptr || !root->mIsFolder)
		{
			out = nullptr;
			return CreateResult::emError;
		}

		if (out = root->FindFile(fileName))
		{
			return CreateResult::emFileExists;
		}

		AssetFile* file = new AssetFile;
		file->mIsFolder = false;
		file->mName = fileName;
		file->mParent = root;

		root->next.push_back(file);

		frame.Save(file->GetAssetFilePath(), &meta);

		out = file;

		Engine::AssetManager::GetInstance()->AddAsset(file->GetAssetFilePath(), meta.GetGuid());

		return CreateResult::emSuccess;
	}

	AssetsFileSystem::CreateResult AssetsFileSystem::CreateAssetFile(const std::filesystem::path& filePath, Engine::SerializerDataFrame& frame, Engine::MetaFrame& meta)
	{
		AssetFile* folderFile = nullptr;
		if (filePath.has_parent_path())
		{
			CreateResult result = CreateFolder(filePath.parent_path(), folderFile);
			if (result == CreateResult::emError)
			{
				return CreateResult::emError;
			}
		}

		return CreateAssetFile(folderFile, filePath.filename().generic_string(), frame, meta);
	}

	AssetsFileSystem::CreateResult AssetsFileSystem::CreateAssetFile(const std::filesystem::path& filePath, Engine::SerializerDataFrame& frame, Engine::MetaFrame& meta, AssetFile*& out)
	{
		AssetFile* folderFile = nullptr;
		if (filePath.has_parent_path())
		{
			CreateResult result = CreateFolder(filePath.parent_path(), folderFile);
			if (result == CreateResult::emError)
			{
				return CreateResult::emError;
			}
		}

		return CreateAssetFile(folderFile, filePath.filename().generic_string(), frame, meta, out);
	}

	AssetsFileSystem::CreateResult AssetsFileSystem::CreateTextFile(AssetFile* root, const std::string& fileName, const char* str, Engine::MetaFrame& meta)
	{
		if (root == nullptr || !root->mIsFolder)
		{
			return CreateResult::emError;
		}

		if (root->FindFile(fileName) != nullptr)
		{
			return CreateResult::emFileExists;
		}

		AssetFile* file = new AssetFile;
		file->mIsFolder = false;
		file->mName = fileName;
		file->mParent = root;

		root->next.push_back(file);

		std::filesystem::path filePath = file->GetAssetFilePath();
		Engine::ProjectFileSystem::GetInstance()->WriteFile(filePath, str);
		meta.Save(filePath.concat(".meta"));

		Engine::AssetManager::GetInstance()->AddAsset(file->GetAssetFilePath(), meta.GetGuid());

		return CreateResult::emSuccess;
	}

	AssetsFileSystem::CreateResult AssetsFileSystem::CreateTextFile(AssetFile* root, const std::string& fileName, const char* str, Engine::MetaFrame& meta, AssetFile*& out)
	{
		if (root == nullptr || !root->mIsFolder)
		{
			out = nullptr;
			return CreateResult::emError;
		}

		if (out = root->FindFile(fileName))
		{
			return CreateResult::emFileExists;
		}

		AssetFile* file = new AssetFile;
		file->mIsFolder = false;
		file->mName = fileName;
		file->mParent = root;

		root->next.push_back(file);

		std::filesystem::path filePath = file->GetAssetFilePath();
		Engine::ProjectFileSystem::GetInstance()->WriteFile(filePath, str);
		meta.Save(filePath.concat(".meta"));

		out = file;

		Engine::AssetManager::GetInstance()->AddAsset(file->GetAssetFilePath(), meta.GetGuid());

		return CreateResult::emSuccess;
	}

	AssetsFileSystem::CreateResult AssetsFileSystem::CreateTextFile(const std::filesystem::path& filePath, const char* str, Engine::MetaFrame& meta)
	{
		AssetFile* folderFile = nullptr;
		if (filePath.has_parent_path())
		{
			CreateResult result = CreateFolder(filePath.parent_path(), folderFile);
			if (result == CreateResult::emError)
			{
				return CreateResult::emError;
			}
		}

		return CreateTextFile(folderFile, filePath.filename().generic_string(), str, meta);

	}

	AssetsFileSystem::CreateResult AssetsFileSystem::CreateTextFile(const std::filesystem::path& filePath, const char* str, Engine::MetaFrame& meta, AssetFile*& out)
	{
		AssetFile* folderFile = nullptr;
		if (filePath.has_parent_path())
		{
			CreateResult result = CreateFolder(filePath.parent_path(), folderFile);
			if (result == CreateResult::emError)
			{
				return CreateResult::emError;
			}
		}

		return CreateTextFile(folderFile, filePath.filename().generic_string(), str, meta, out);
	}

	AssetsFileSystem::DeleteResult AssetsFileSystem::DeleteAssetFile(AssetFile* root)
	{
		if (root == nullptr)
		{
			return DeleteResult::emError;
		}

		if (root->mIsFolder)
		{
			return DeleteFolder(root);
		}

		if (root->mParent == nullptr)
		{
			return DeleteResult::emError;
		}

		AssetFile* parent = root->mParent;

		std::erase(parent->next, root);

		std::filesystem::path filePath = root->GetAssetFilePath();
		Engine::AssetManager::GetInstance()->RemoveAsset(filePath);

		std::filesystem::path actualPath = Engine::ProjectFileSystem::GetInstance()->GetActualPath(filePath);
		std::filesystem::remove(actualPath);
		std::filesystem::remove(actualPath.concat(".meta"));

		delete root;

		return DeleteResult::emSuccess;
	}

	AssetsFileSystem::DeleteResult AssetsFileSystem::DeleteAssetFile(const std::filesystem::path& filePath)
	{
		AssetFile* file = GetAssetFile(filePath);
		if (file == nullptr)
		{
			return DeleteResult::emFileNotExists;
		}

		return DeleteAssetFile(file);
	}

	AssetsFileSystem::DeleteResult AssetsFileSystem::DeleteFolder(AssetFile* root)
	{
		if (!root->mIsFolder || root->mParent == nullptr)
		{
			return DeleteResult::emError;
		}

		auto it = root->next.begin();

		while (it != root->next.end())
		{
			AssetFile* file = *it;
			if (file == nullptr)
			{
				continue;
			}

			if (file->mIsFolder)
			{
				DeleteFolder(file);
			}
			else
			{
				DeleteAssetFile(file);
			}

			it = root->next.begin();
		}

		AssetFile* parent = root->mParent;

		std::erase(parent->next, root);

		std::filesystem::path filePath = root->GetAssetFilePath();
		std::filesystem::remove(Engine::ProjectFileSystem::GetInstance()->GetActualPath(filePath));
		
		delete root;

		return DeleteResult::emSuccess;
	}


	AssetsFileSystem::MoveResult AssetsFileSystem::AssetFileMove(AssetFile* oldFile, AssetFile* targetFolder)
	{
		if (oldFile == nullptr || oldFile->mParent == nullptr || targetFolder == nullptr)
		{
			return MoveResult::emError;
		}

		if (!targetFolder->mIsFolder)
		{
			return MoveResult::emTargetNotFolder;
		}

		std::filesystem::path oldFilePath = oldFile->GetAssetFilePath();
		std::filesystem::path targetFilePath = targetFolder->GetAssetFilePath() / oldFile->mName;

		if (oldFile->mIsFolder)
		{
			PathRevise(oldFile, "", targetFolder->GetAssetFilePath());
		}
		else
		{
			Engine::AssetManager::GetInstance()->MoveAsset(oldFilePath, targetFilePath);

		}

		std::erase(oldFile->mParent->next, oldFile);
		oldFile->mParent = targetFolder;
		targetFolder->next.push_back(oldFile);

		std::filesystem::rename(Engine::ProjectFileSystem::GetInstance()->GetActualPath(oldFilePath), Engine::ProjectFileSystem::GetInstance()->GetActualPath(targetFilePath));

		return MoveResult::emSuccess;
	}

	AssetsFileSystem::MoveResult AssetsFileSystem::AssetFileMove(const std::filesystem::path& oldFilePath, const std::filesystem::path& targetFolderPath)
	{
		return AssetFileMove(GetAssetFile(oldFilePath), GetAssetFile(targetFolderPath));
	}

	void AssetsFileSystem::PathRevise(AssetFile* file, const std::filesystem::path& temporaryPath, const std::filesystem::path& absoluteRootPath)
	{
		if (file->mIsFolder)
		{
			for (AssetFile* next : file->next)
			{
				PathRevise(next, temporaryPath / next->mName, absoluteRootPath);
			}

			return;
		}
		
		Engine::AssetManager::GetInstance()->MoveAsset(file->GetAssetFilePath(), absoluteRootPath / temporaryPath);
	}

	AssetsFileSystem::RenameResult AssetsFileSystem::AssetFileRename(AssetFile* file, std::string newName)
	{
		if (file == nullptr)
		{
			return RenameResult::emError;
		}
		
		std::filesystem::path filePath = file->GetAssetFilePath();
		std::filesystem::path newfilePath = file->GetAssetFilePath();

		newfilePath.replace_filename(newName);

		if (file->mIsFolder)
		{
			PathRevise(file, "", file->GetParentPath() / newName);
		}
		else
		{
			Engine::AssetManager::GetInstance()->MoveAsset(filePath, newfilePath);
		}

		file->mName = newName;

		std::filesystem::rename(Engine::ProjectFileSystem::GetInstance()->GetActualPath(filePath), Engine::ProjectFileSystem::GetInstance()->GetActualPath(newfilePath));

		if (!file->mIsFolder)
		{
			filePath += ".meta";
			newfilePath += ".meta";
			std::filesystem::rename(Engine::ProjectFileSystem::GetInstance()->GetActualPath(filePath), Engine::ProjectFileSystem::GetInstance()->GetActualPath(newfilePath));
		}

		return RenameResult::emSuccess;
	}

	AssetsFileSystem::RenameResult AssetsFileSystem::AssetFileRename(const std::filesystem::path& filePath, const std::string& newName)
	{
		return AssetFileRename(GetAssetFile(filePath), newName);
	}

	void AssetsFileSystem::dfsCreateFile(AssetFile* root, const std::filesystem::path& currentPath)
	{
		for (const auto& file : std::filesystem::directory_iterator(currentPath))
		{
			if (file.path().extension() == ".meta")
			{
				continue;
			}

			AssetFile* node = new AssetFile;
			node->mParent = root;
			node->mName = file.path().filename().generic_string();
			if (file.is_directory())
			{
				node->mIsFolder = true;
				dfsCreateFile(node, file);
			}
			root->next.push_back(node);
		}

	}
}
