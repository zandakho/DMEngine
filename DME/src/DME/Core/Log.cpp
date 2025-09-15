#include "dmepch.h"

#include "DME/Core/Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace DME
{
	std::shared_ptr<spdlog::logger> Log::s_Core;
	std::shared_ptr<spdlog::logger> Log::s_Client;
	std::shared_ptr<LogImGuiSink> Log::m_ImGuiSink;

	void Log::Init()
	{
		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("DME.log", true));

		m_ImGuiSink = std::make_shared<LogImGuiSink>();
		logSinks.emplace_back(m_ImGuiSink);

		logSinks[0]->set_pattern("%^[%T] %n: %v%$");
		logSinks[1]->set_pattern("[%T] [%l] %n: %v");
		logSinks[2]->set_pattern("[%T] [%l] %n: %v");

		s_Core = std::make_shared<spdlog::logger>("DME", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_Core);
		s_Core->set_level(spdlog::level::trace);
		s_Core->flush_on(spdlog::level::trace);

		s_Client = std::make_shared<spdlog::logger>("APP", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_Client);
		s_Client->set_level(spdlog::level::trace);
		s_Client->flush_on(spdlog::level::trace);

	}

}


