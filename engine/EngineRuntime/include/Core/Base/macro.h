#pragma once
#include <cassert>
#include "EngineRuntime/include/Core/Log/LogSystem.h"

#ifdef _DEBUG
#define ASSERT(...) assert(__VA_ARGS__)
#else 
#define ASSERT(...)
#endif

#define LOG_HANDEL(LOG_LEVEL, String, ...) \
	Engine::LogSystem::GetInstance()->Log(LOG_LEVEL, String, __VA_ARGS__);

#define LOG_FATAL_HANDEL(LOG_LEVEL, String, ...)							\
	Engine::LogSystem::GetInstance()->Log(LOG_LEVEL, String,  __VA_ARGS__);	\
	ASSERT(0);

#define LOG_DEBUG(String, ...) \
	LOG_HANDEL(spdlog::level::debug, String, __VA_ARGS__);

#define LOG_INFO(String, ...) \
	LOG_HANDEL(spdlog::level::info, String, __VA_ARGS__);

#define LOG_WARN(String, ...) \
	LOG_HANDEL(spdlog::level::warn, String, __VA_ARGS__);

#define LOG_ERROR(String, ...) \
	LOG_HANDEL(spdlog::level::err, String, __VA_ARGS__);

#define LOG_FATAL(String, ...) \
	LOG_FATAL_HANDEL(spdlog::level::critical, String, __VA_ARGS__);
