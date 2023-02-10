#include "EngineRuntime/include/EngineRuntime.h"
#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Function/Framework/World/WorldManager.h"
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

		if (false)
		{
			{
				if (HMODULE mod = LoadLibraryA("renderdoc.dll"))
				{
					pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
					int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void**)&rdoc_api);
					assert(ret == 1);
					LOG_INFO("注入RenderDoc成功");
				}
				else
				{
					LOG_ERROR("注入RenderDoc失败 {0}", GetLastError());
				}
			}
		}
		
		if (!NativeFileSystem::GetInstance()->Initialize(info))
		{
			LOG_ERROR("文件系统初始化失败");
			return false;
		}

		LOG_INFO("文件系统初始化完成");

		if (rdoc_api != nullptr)
		{
			std::string generic_string = EngineFileSystem::GetInstance()->GetActualPath("RenderDoc/").generic_string();

			rdoc_api->SetCaptureFilePathTemplate(generic_string.c_str());
			LOG_INFO("设置截帧保存模板 {0}", generic_string.c_str());
		}

		if (!ConfigManager::GetInstance()->Initialize(info->enginePath))
		{
			LOG_ERROR("配置系统初始化失败");
			return false;
		}

		LOG_INFO("配置系统初始化完成");

		if(!AssetManager::GetInstance()->Initialize(info->workspacePath))
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

		if (!WorldManager::GetInstance()->Initialize())
		{
			LOG_ERROR("游戏场景初始化失败");
			return false;
		}

		LOG_INFO("游戏场景初始化完成");

		mLastTickTimePoint = std::chrono::steady_clock::now();

		return true;
	}

	void EngineRuntime::Finalize()
	{
		WorldManager::GetInstance()->Finalize();
		RenderSystem::GetInstance()->Finalize();
		WindowSystem::GetInstance()->Finalize();
		AssetManager::GetInstance()->Finalize();
		ConfigManager::GetInstance()->Finalize();
		NativeFileSystem::GetInstance()->Finalize();
		LogSystem::GetInstance()->Finalize();
	}

	bool EngineRuntime::Tick(float deltaTime, bool isEditorMode)
	{
		/*
		if (!isEditorMode)
		{
			
		}
		*/
		WorldManager::GetInstance()->Tick(deltaTime);
		RenderSystem::GetInstance()->Tick(deltaTime, isEditorMode);
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
