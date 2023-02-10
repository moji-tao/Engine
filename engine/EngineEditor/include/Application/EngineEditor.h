#pragma once
#include "EngineEditor/include/Application/EditorConfig.h"
#include "EngineEditor/include/Application/SubSystem/AssetsFileSystem.h"
#include "EngineEditor/include/Application/SubSystem/EditorInputSystem.h"
#include "EngineEditor/include/Application/SubSystem/EditorSceneSystem.h"
#include "EngineEditor/include/UI/EditorUI.h"

namespace Editor
{
	class EngineEditor : public Engine::ISingleton<EngineEditor>
	{
	public:
		EngineEditor() = default;

		~EngineEditor() = default;

	public:
		bool Initialize(const EditorConfig* config);

		void Finalize();

		void Run();

		AssetsFileSystem* GetFileSystem();

		EditorInputSystem* GetInputSystem();

		Engine::RenderCamera* GetEditorCamera();

		bool OpenFileSelector(std::filesystem::path& filePath) const;

		void OpenExplorer(const std::filesystem::path& folderPath) const;

		bool mIsEditorMode = true;

	private:
		static void FilesDrop(int fileCount, const char** filePath);

	protected:
		bool mIsQuit = false;

		std::filesystem::path mEnginePath;
		std::filesystem::path mWorkSpacePath;

		EditorUI* mEditorUI;

	public:
		EditorConfig Config;

	private:
		AssetsFileSystem* mAssetsFileSystem = nullptr;

		EditorSceneSystem* mEditorSceneSystem = nullptr;

		EditorInputSystem* mEditorInputSystem = nullptr;

	private:
		
	};
}
