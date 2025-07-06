#include "dmepch.h"

// --- DME/Core/ ---------------------------------------------//
#include "DME/Core/Log.h"										//
// -------------------------------------------------------------//

// --- Dependencies --------------------------------------------//
#include <spdlog/sinks/stdout_color_sinks.h>					//
// -------------------------------------------------------------//

namespace DME
{
	std::shared_ptr<spdlog::logger> Log::c_Client;
	std::shared_ptr<spdlog::logger> Log::c_Core;

	void Log::Init()
	{
		spdlog::set_pattern("%^IN: [%D] - [%T] MESSAGE: <<%v>>\nLOGGER: [%n] | LEVEL: [%l]\n%$");
		c_Core = spdlog::stdout_color_mt("DME");
		c_Core->set_level(spdlog::level::trace);

		c_Client = spdlog::stdout_color_mt("APP");
		c_Client->set_level(spdlog::level::trace);
	}

}


