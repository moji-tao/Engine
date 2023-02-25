#include <iostream>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include "EngineEditor/include/Application/EngineEditor.h"

Editor::EditorConfig Initialize(int argc, char** argv)
{
	if (argc != 2)
	{
		throw "process not specified workspace!";
	}
	boost::property_tree::ptree root_node;
	boost::property_tree::ptree editorField;
	boost::property_tree::read_ini("./EngineEditor.ini", root_node);
	editorField = root_node.get_child("Editor");

	Editor::EditorConfig config;
	config.enginePath = std::filesystem::path(argv[0]).parent_path();
	config.workspacePath = argv[1];
	config.WindowWidth = editorField.get<uint16_t>("WindowWidth");
	config.WindowHeight = editorField.get<uint16_t>("WindowHeight");
	config.Title = editorField.get<std::string>("Title");
	config.editorResourceConfig.editorFontPath = editorField.get<std::filesystem::path>("FontFile");
	config.editorResourceConfig.editorFileIcon = editorField.get<std::filesystem::path>("FileIcon");
	config.editorResourceConfig.editorFolderIcon = editorField.get<std::filesystem::path>("FolderIcon");
	config.editorResourceConfig.editorImageIcon = editorField.get<std::filesystem::path>("ImageIcon");
	config.editorResourceConfig.editorIntegralIcon = editorField.get<std::filesystem::path>("IntegralIcon");

	return config;
}

int main(int argc, char** argv)
{
	try
	{
		Editor::EditorConfig config = Initialize(argc, argv);
		
		Editor::EngineEditor::GetInstance()->Initialize(&config);

		Editor::EngineEditor::GetInstance()->Run();

		Editor::EngineEditor::GetInstance()->Finalize();
	}
	catch (std::string& e)
	{
		std::cout << e << std::endl;
	} 
	
	return 0;
}