#pragma once
#include <filesystem>

namespace Editor
{
	struct ApplicationDesc
	{
		std::filesystem::path enginePath;
		std::filesystem::path workspacePath;
	};

	class IApplication
	{
	public:
		IApplication() = default;

		virtual ~IApplication() = default;

	public:
		virtual bool Initialize(const ApplicationDesc* config) = 0;

		virtual void Finalize() = 0;

		virtual void Run() = 0;
	};
}
