#include "EngineRuntime/include/EngineRuntime.h"
#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Function/Render/RenderSystem.h"
#include "EngineRuntime/include/Function/Window/WindowSystem.h"
#include "EngineRuntime/include/Platform/FileSystem/FileSystem.h"
#include "EngineRuntime/include/Resource/AssetManager/AssetManager.h"
#include "EngineRuntime/include/Resource/ConfigManager/ConfigManager.h"

namespace Engine
{
	EngineRuntime::~EngineRuntime()
	{ }

	bool EngineRuntime::Initialize(InitConfig* info)
	{
		if (!LogSystem::GetInstance()->Initialize(info->workspacePath))
		{
			return false;
		}

		LOG_INFO("日志系统初始化完成");

		if (!NativeFileSystem::GetInstance()->Initialize(info))
		{
			LOG_ERROR("文件系统初始化失败");
			return false;
		}

		LOG_INFO("文件系统初始化完成");

		if (!ConfigManager::GetInstance()->Initialize(info->enginePath))
		{
			LOG_ERROR("配置系统初始化失败");
			return false;
		}

		LOG_INFO("配置系统初始化完成");

		if(!AssetManager::GetInstance()->Initialize())
		{
			LOG_ERROR("资源系统初始化失败");
		}

		LOG_INFO("资源系统初始化完成");


		if (!WindowSystem::GetInstance()->Initialize(info))
		{
			LOG_ERROR("窗口初始化失败");
			return false;
		}

		LOG_INFO("窗口初始化完成");

		info->windowHandle = WindowSystem::GetInstance();

		if (!RenderSystem::GetInstance()->Initialize(info))
		{
			LOG_ERROR("渲染系统初始化失败");
			return false;
		}

		LOG_INFO("渲染系统初始化完成");

		mLastTickTimePoint = std::chrono::steady_clock::now();

		return true;
	}

	void EngineRuntime::Finalize()
	{
		RenderSystem::GetInstance()->Finalize();
		WindowSystem::GetInstance()->Finalize();
		AssetManager::GetInstance()->Finalize();
		ConfigManager::GetInstance()->Finalize();
		NativeFileSystem::GetInstance()->Finalize();
		LogSystem::GetInstance()->Finalize();
	}

	bool EngineRuntime::Tick(float deltaTime)
	{
		RenderSystem::GetInstance()->Tick(deltaTime);
		WindowSystem::GetInstance()->Tick(deltaTime);

		return !WindowSystem::GetInstance()->ShouldClose();
	}

	float EngineRuntime::GetDeltaTime()
	{
		std::chrono::steady_clock::time_point nowTickTimePoint = std::chrono::steady_clock::now();

		std::chrono::duration<float> timeSpan = std::chrono::duration_cast<std::chrono::duration<float>>(nowTickTimePoint - mLastTickTimePoint);

		float deltaTime = timeSpan.count();

		mLastTickTimePoint = nowTickTimePoint;

		return deltaTime;
	}

}
