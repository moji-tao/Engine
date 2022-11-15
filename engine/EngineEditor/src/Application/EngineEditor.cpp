#include <filesystem>
#include "EngineEditor/include/Application/EngineEditor.h"
#include "EngineRuntime.h"
#include "EngineRuntime/include/Core/Base/macro.h"
#include "Platform/FileSystem/FileSystem.h"
#include "EngineRuntime/include/Resource/AssetManager/AssetManager.h"
#include "EngineRuntime/include/Function/Window/WindowSystem.h"
#include "EngineRuntime/include/Function/Render/RenderSystem.h"

namespace Editor
{
	bool EngineEditor::Initialize(const ApplicationDesc* config)
	{
		Engine::InitConfig initConfig;

		AnalysisConfig(config, initConfig);

		if (!Engine::EngineRuntime::GetInstance()->Initialize(&initConfig))
		{
			mIsQuit = true;
			return false;
		}

		
		mEditorUI = std::make_shared<EditorUI>();

		Engine::WindowUIInitInfo info;
		info.windowSystem = Engine::WindowSystem::GetInstance();
		info.renderSystem = Engine::RenderSystem::GetInstance();
		
		mEditorUI->Initialize(info);
		

		Engine::RenderSystem::GetInstance()->ResizeEngineContentViewport(420, 180, 1080, 720);

		Engine::WindowSystem::GetInstance()->RegisterFilesDropCallback(FilesDrop);

		return true;
	}

	void EngineEditor::Finalize()
	{
		mEditorUI = nullptr;
		Engine::EngineRuntime::GetInstance()->Finalize();
		mIsQuit = true;
	}

	void EngineEditor::Run()
	{
		while (!mIsQuit)
		{
			float deltaTime = Engine::EngineRuntime::GetInstance()->GetDeltaTime();
			mIsQuit = !Engine::EngineRuntime::GetInstance()->Tick(deltaTime);
		}

	}

	void EngineEditor::AnalysisConfig(const ApplicationDesc* config, Engine::InitConfig& initConfig)
	{
		initConfig.windowHandle = nullptr;
		initConfig.WindowWidth = 1920;
		initConfig.WindowHeight = 1080;
		initConfig.Title = "MINI引擎";
		initConfig.enginePath = config->enginePath;
		initConfig.workspacePath = config->workspacePath;
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
}
