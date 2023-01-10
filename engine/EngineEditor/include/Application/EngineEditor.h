#pragma once
#include "EngineEditor/include/Application/EditorConfig.h"
#include "EngineEditor/include/UI/EditorUI.h"

namespace Editor
{
	class EngineEditor
	{
	public:
		EngineEditor() = default;

		~EngineEditor() = default;

	public:
		bool Initialize(const EditorConfig* config);

		void Finalize();

		void Run();

		bool OpenFileSelector(std::filesystem::path& filePath) const;

		void OpenExplorer(const std::filesystem::path& folderPath) const;

		void DeleteFileOrFolder(const std::filesystem::path& filePath) const;

		void RenameFileOrFolder(const std::filesystem::path& oldPath, const std::filesystem::path& newPath);

		void CreateLuaScript(const std::filesystem::path& scriptPath);

	private:

	private:
		static void FilesDrop(int fileCount, const char** filePath);

	protected:
		bool mIsQuit = false;

		std::filesystem::path mEnginePath;
		std::filesystem::path mWorkSpacePath;

		std::shared_ptr<EditorUI> mEditorUI;

	public:
		EditorConfig Config;
	};
}
