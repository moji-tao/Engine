#pragma once
#include <filesystem>

namespace Editor
{
	struct EditorResourceConfig
	{
		std::filesystem::path editorFontPath;
		std::filesystem::path editorFolderIcon;
		std::filesystem::path editorFileIcon;
		std::filesystem::path editorImageIcon;
		std::filesystem::path editorIntegralIcon;
	};

	struct EditorConfig
	{
		uint16_t WindowWidth;
		uint16_t WindowHeight;
		std::string Title;
		EditorResourceConfig editorResourceConfig;
		std::filesystem::path enginePath;
		std::filesystem::path workspacePath;
	};
}
