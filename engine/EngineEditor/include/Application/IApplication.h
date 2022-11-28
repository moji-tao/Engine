#pragma once
#include <filesystem>

namespace Editor
{
	struct EditorConfig
	{
		uint16_t WindowWidth;
		uint16_t WindowHeight;
		std::string Title;
		std::filesystem::path editorFontPath;
		std::filesystem::path enginePath;
		std::filesystem::path workspacePath;
	};

	class IApplication
	{
	public:
		IApplication() = default;

		virtual ~IApplication() = default;

	public:
		virtual bool Initialize(const EditorConfig* config) = 0;

		virtual void Finalize() = 0;

		virtual void Run() = 0;
	};
}
