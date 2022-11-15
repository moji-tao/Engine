#include <iostream>

#include "EngineEditor/include/Application/EngineEditor.h"

int main(int argc, char** argv)
{
	try
	{
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
	}
	catch (std::runtime_error& e)
	{

	}


	return 0;
}