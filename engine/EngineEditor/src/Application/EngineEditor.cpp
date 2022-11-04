#include "EngineEditor/include/Application/EngineEditor.h"
#include "EngineRuntime.h"

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

		return true;
	}

	void EngineEditor::Finalize()
	{
		Engine::EngineRuntime::GetInstance()->Finalize();
		mIsQuit = true;
	}

	void EngineEditor::Run()
	{
		float deltaTime = Engine::EngineRuntime::GetInstance()->GetDeltaTime();

		while (!mIsQuit)
		{
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

}
