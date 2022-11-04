#include <iostream>

#include "EngineEditor/include/Application/EngineEditor.h"

int main(int argc, char** argv)
{
	/*
	try
	{
		char cwd[256] = { };
		_getcwd(cwd, 256);
		InitConfig initConfig;
		initConfig.WindowWidth = 800;
		initConfig.WindowHeight = 600;
		initConfig.windowHandle = nullptr;
		initConfig.Title = "MINI引擎";
		initConfig.EnginePath = cwd;
		initConfig.workspacePath = cwd;

		WindowSystem window;
		WindowCreateInfo info;
		info.m_nWidth = initConfig.WindowWidth;
		info.m_nHeight = initConfig.WindowHeight;
		info.m_bFocusMode = false;
		info.m_pTitle = initConfig.Title;
		window.Initialize(info);

		initConfig.windowHandle = &window;

		RHI* render = new DirectXRender;
		render->Initialize(&initConfig);

		while (!window.ShouldClose())
		{
			render->Tick();
			window.PollEvents();
		}
		delete render;

	}
	catch (std::runtime_error& e)
	{
		return -1;
	}
	*/

	if (argc != 2)
	{
		std::cout << "process not specified workspace!\n";
		return -1;
	}

	std::filesystem::path engineDir = argv[0];

	engineDir = engineDir.parent_path();

	std::filesystem::path workspaceDir = argv[1];

	if (!std::filesystem::is_directory(argv[1]))
	{
		std::cout << "workspace \"" << argv[1] << "\" don't exist or not directory\n";
		return -2;
	}

	std::cout << "engine run\nEnginePath:" << engineDir << "\nWorkSpacePath: " << workspaceDir << std::endl;

	Editor::ApplicationDesc desc = { engineDir, workspaceDir };

	Editor::EngineEditor editor;

	editor.Initialize(&desc);

	editor.Run();

	editor.Finalize();

	return 0;
}