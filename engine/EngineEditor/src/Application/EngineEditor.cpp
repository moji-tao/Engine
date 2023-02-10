#include <filesystem>
#include <Windows.h>
#include "EngineEditor/include/Application/EngineEditor.h"
#include "EngineRuntime/include/EngineRuntime.h"
#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Platform/FileSystem/FileSystem.h"
#include "EngineRuntime/include/Function/Window/WindowSystem.h"
#include "EngineRuntime/include/Function/Render/RenderSystem.h"
#include "EngineRuntime/include/Resource/AssetManager/AssetManager.h"

namespace Editor
{
	bool EngineEditor::Initialize(const EditorConfig* config)
	{
		Engine::InitConfig initConfig;
		initConfig.WindowWidth = config->WindowWidth;
		initConfig.WindowHeight = config->WindowHeight;
		initConfig.Title = config->Title.c_str();
		mEnginePath = initConfig.enginePath = config->enginePath;
		mWorkSpacePath = initConfig.workspacePath = config->workspacePath;

		Config = *config;

		if (!Engine::EngineRuntime::GetInstance()->Initialize(&initConfig))
		{
			mIsQuit = true;
			return false;
		}

		mAssetsFileSystem = new AssetsFileSystem(config->workspacePath);
		mEditorSceneSystem = new EditorSceneSystem(this);
		mEditorInputSystem = new EditorInputSystem(this);

		mEditorUI = new EditorUI(this);

		Engine::WindowUIInitInfo info;
		info.windowSystem = Engine::WindowSystem::GetInstance();
		info.renderSystem = Engine::RenderSystem::GetInstance();
		
		mEditorUI->Initialize(info);

		Engine::WindowSystem::GetInstance()->RegisterFilesDropCallback(FilesDrop);

		return true;
	}

	void EngineEditor::Finalize()
	{
		delete mEditorSceneSystem;
		mEditorSceneSystem = nullptr;

		delete mEditorInputSystem;
		mEditorInputSystem = nullptr;

		delete mAssetsFileSystem;
		mAssetsFileSystem = nullptr;

		delete mEditorUI;
		mEditorUI = nullptr;

		Engine::EngineRuntime::GetInstance()->Finalize();
		mIsQuit = true;
	}

	void EngineEditor::Run()
	{
		while (!mIsQuit)
		{
			mEditorSceneSystem->Tick();
			mEditorInputSystem->Tick();

			float deltaTime = Engine::EngineRuntime::GetInstance()->GetDeltaTime();
			mIsQuit = !Engine::EngineRuntime::GetInstance()->Tick(deltaTime, mIsEditorMode);
		}

	}

	AssetsFileSystem* EngineEditor::GetFileSystem()
	{
		return mAssetsFileSystem;
	}

	EditorInputSystem* EngineEditor::GetInputSystem()
	{
		return mEditorInputSystem;
	}

	Engine::RenderCamera* EngineEditor::GetEditorCamera()
	{
		return mEditorSceneSystem->GetEditorRenderCamera();
	}

	void EngineEditor::FilesDrop(int fileCount, const char** filePath)
	{
		LOG_INFO("检测到文件拖入");

		/*
		for (int i = 0; i < fileCount; ++i)
		{
			LOG_INFO("有文件拖入 {0}", filePath[i]);
			std::filesystem::path ph(filePath[i]);
			if(!std::filesystem::is_directory(ph))
			{
				std::filesystem::path toPath = Engine::FileSystem::GetInstance()->GetProjectAssetsPath() / ph.filename();
				if(std::filesystem::exists(toPath))
				{
					LOG_INFO("文件已存在 {0}", toPath.string().c_str());

					continue;
				}
				std::filesystem::copy_file(ph, toPath);
				guid = Engine::AssetManager::GetInstance()->LoadAsset("Assets/" / toPath.filename());

				LOG_INFO("GUID已生成 {0}", guid.c_str());
			}
		}
		*/

	}

	bool EngineEditor::OpenFileSelector(std::filesystem::path& filePath) const
	{
		OPENFILENAME ofn;			// 公共对话框结构
		WCHAR szFile[MAX_PATH];		// 保存获取文件名称的缓冲区   
		ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
		ofn.lStructSize = sizeof(OPENFILENAMEA);
		ofn.hwndOwner = (HWND)Engine::WindowSystem::GetInstance()->GetWindowHandle(true);
		ofn.lpstrFile = szFile;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = L"All files (*.*)\0*.*\0"; //过滤规则
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = mWorkSpacePath.c_str();	//指定默认路径
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		ofn.lpstrTitle = L"导入新资产";

		if (GetOpenFileName(&ofn))
		{
			//显示选择的文件。 
			filePath = ofn.lpstrFile;

			return true;
		}

		return false;
	}

	void EngineEditor::OpenExplorer(const std::filesystem::path& folderPath) const
	{
		auto pathses = Engine::ProjectFileSystem::GetInstance()->GetActualPath(folderPath);
		ASSERT(std::filesystem::is_directory(pathses));
		
		ShellExecute(nullptr, L"open", L"explorer.exe", pathses.c_str(), nullptr, SW_SHOWNORMAL);
	}
}
