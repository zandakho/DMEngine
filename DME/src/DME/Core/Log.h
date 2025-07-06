#pragma once

#include "DME/Core/Base.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace DME {

	class Log
	{
	private:

		static std::shared_ptr<spdlog::logger> c_Client;
		static std::shared_ptr<spdlog::logger> c_Core;


	public:

		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetClientLog() { return c_Client; }
		inline static std::shared_ptr<spdlog::logger>& GetCoreLog() { return c_Core; }


	};
}


#define DME_CORE_CRITICAL(...)		::DME::Log::GetCoreLog()->critical(__VA_ARGS__)
#define DME_CORE_ERROR(...)			::DME::Log::GetCoreLog()->error(__VA_ARGS__)
#define DME_CORE_WARNING(...)		::DME::Log::GetCoreLog()->warn(__VA_ARGS__)
#define DME_CORE_INFO(...)			::DME::Log::GetCoreLog()->info(__VA_ARGS__)
#define DME_CORE_TRACE(...)			::DME::Log::GetCoreLog()->trace(__VA_ARGS__)

#define DME_CRITICAL(...)			::DME::Log::GetClientLog()->critical(__VA_ARGS__)
#define DME_ERROR(...)				::DME::Log::GetClientLog()->error(__VA_ARGS__)
#define DME_WARNING(...)				::DME::Log::GetClientLog()->warn(__VA_ARGS__)
#define DME_INFO(...)				::DME::Log::GetClientLog()->info(__VA_ARGS__)
#define DME_TRACE(...)				::DME::Log::GetClientLog()->trace(__VA_ARGS__)
