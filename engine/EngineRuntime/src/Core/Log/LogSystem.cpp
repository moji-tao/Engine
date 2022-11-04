#include "EngineRuntime/include/Core/Log/LogSystem.h"

#include "spdlog/async.h"
#include "spdlog/async_logger.h"
#include "spdlog/spdlog-inl.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Engine
{
	using namespace spdlog;

	bool LogSystem::Initialize(const std::filesystem::path& _workspacePath)
	{
		size_t MAX_FILE_SIZE = 1024 * 1024 * 100; //  100Mb
		size_t MAX_FILE_COUNT = 10;

		std::filesystem::path workspacePath = _workspacePath / "Logs" / "Log.txt";

		std::vector<sink_ptr> sinks;
		sinks.push_back(std::make_shared<sinks::stdout_color_sink_mt>());
		sinks.push_back(std::make_shared<sinks::rotating_file_sink_mt>(
			workspacePath.string(), MAX_FILE_SIZE, MAX_FILE_COUNT));

		sinks[0]->set_pattern("[%Y-%m-%d %T][%^%-l%$][%s:%#] %v");
		sinks[1]->set_pattern("[%Y-%m-%d %T][%-l][%s:%#] %v");

		const sinks_init_list sink_list = { sinks[0], sinks[1] };

		init_thread_pool(8192, 1);

		m_logger = std::make_shared<async_logger>("muggle_logger",
			sink_list.begin(),
			sink_list.end(),
			thread_pool(),
			async_overflow_policy::block);
		m_logger->set_level(level::trace);

		register_logger(m_logger);

		return true;
	}

	void LogSystem::Finalize()
	{
		m_logger->flush();
		drop_all();
	}
}
