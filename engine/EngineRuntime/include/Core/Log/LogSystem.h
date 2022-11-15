#pragma once
#include <cstdint>
#include <filesystem>
#include <memory>
#include "EngineRuntime/include/Framework/Interface/ISingleton.h"

#include "spdlog/logger.h"
#include "spdlog/fmt/ostr.h"

#include "spdlog/sinks/rotating_file_sink.h"
#include <source_location>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Engine
{
	class LogSystem : public ISingleton<LogSystem>
	{
	public:
		bool Initialize(const std::filesystem::path& _workspacePath);

		void Finalize();


#if (defined(_MSC_VER) && _MSVC_LANG >= 202002L) || (__cplusplus >= 202002L)
	private:
		static constexpr spdlog::source_loc GetLogSourceLocation(const std::source_location& location)
		{
			return spdlog::source_loc{ location.file_name(), static_cast<std::int32_t>(location.line()), location.function_name() };
		}

	public:
		struct format_with_location
		{
			std::string_view value;
			spdlog::source_loc loc;

			template <typename String>
			format_with_location(const String& s, const std::source_location& location = std::source_location::current())
				: value{ s }, loc{ GetLogSourceLocation(location) } { }
		};
		template <typename... Args>
		void Log(spdlog::level::level_enum level, format_with_location fmt_, Args&&... args)
		{
			m_logger->log(fmt_.loc, level, fmt::runtime(fmt_.value),
				std::forward<Args>(args)...);
		}
#else
		template <typename... Args>
		void Log(spdlog::level::level_enum level, Args&&... args)
		{
			m_logger->log(level, std::forward<Args>(args)...);
		}

#endif

		template <typename... Args>
		void FatalCallback(Args&&... args)
		{
			const std::string format_str = fmt::format(std::forward<Args>(args)...);
			throw std::runtime_error(format_str);
		}


	private:
		std::shared_ptr<spdlog::logger> m_logger;
	};
}

