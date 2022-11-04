#pragma once
#include <cstdint>
#include <filesystem>

namespace Engine
{
	struct InitConfig
	{
		uint16_t WindowWidth;
		uint16_t WindowHeight;
		const char* Title;
		std::filesystem::path enginePath;
		std::filesystem::path workspacePath;
		void* windowHandle;
	};

	class IRuntimeModule
	{
	public:
		IRuntimeModule() = default;

		virtual ~IRuntimeModule() = default;

	public:
		virtual bool Initialize(InitConfig* info) = 0;

		virtual	void Finalize() = 0;

		virtual bool Tick(float deltaTime) = 0;
	};
}
