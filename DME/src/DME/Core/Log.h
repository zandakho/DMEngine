#pragma once

#include "DME/Core/LogImGuiSink.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace DME {

	class Log
	{
	private:

		static std::shared_ptr<spdlog::logger> s_Core;
		static std::shared_ptr<spdlog::logger> s_Client;

	public:

		static void Init();

		static std::shared_ptr<spdlog::logger>& GetCoreLog() { return s_Core; }
		static std::shared_ptr<spdlog::logger>& GetClientLog() { return s_Client; }
		static std::shared_ptr<LogImGuiSink> m_ImGuiSink;

	};
}

namespace DME::LogSettings
{

	inline bool m_GlobalCoreLogger = true;
	inline bool m_GlobalLogger = true;

	inline bool m_CoreCriticalLogger = true;
	inline bool m_CoreErrorLogger = true;
	inline bool m_CoreWarningLogger = true;
	inline bool m_CoreInfoLogger = true;
	inline bool m_CoreTraceLogger = true;

	inline bool m_CriticalLogger = true;
	inline bool m_ErrorLogger = true;
	inline bool m_WarningLogger = true;
	inline bool m_InfoLogger = true;
	inline bool m_TraceLogger = true;

	inline spdlog::level::level_enum m_LogFilter = spdlog::level::trace;

}


template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
	return os << glm::to_string(quaternion);
}


#define DME_CORE_CRITICAL(...)		if (DME::LogSettings::m_GlobalCoreLogger) { if (DME::LogSettings::m_CoreCriticalLogger) { DME::Log::GetCoreLog()->critical(__VA_ARGS__); } }		
#define DME_CORE_ERROR(...)			if (DME::LogSettings::m_GlobalCoreLogger) { if (DME::LogSettings::m_CoreErrorLogger)	{ DME::Log::GetCoreLog()->error(__VA_ARGS__); } }
#define DME_CORE_WARNING(...)		if (DME::LogSettings::m_GlobalCoreLogger) { if (DME::LogSettings::m_CoreWarningLogger)	{ DME::Log::GetCoreLog()->warn(__VA_ARGS__); } }
#define DME_CORE_INFO(...)			if (DME::LogSettings::m_GlobalCoreLogger) { if (DME::LogSettings::m_CoreInfoLogger)		{ DME::Log::GetCoreLog()->info(__VA_ARGS__); } }
#define DME_CORE_TRACE(...)			if (DME::LogSettings::m_GlobalCoreLogger) { if (DME::LogSettings::m_CoreTraceLogger)	{ DME::Log::GetCoreLog()->trace(__VA_ARGS__); } }

#define DME_CRITICAL(...)			if (DME::LogSettings::m_GlobalLogger) { if (DME::LogSettings::m_CriticalLogger)		{ DME::Log::GetClientLog()->critical(__VA_ARGS__); } }
#define DME_ERROR(...)				if (DME::LogSettings::m_GlobalLogger) { if (DME::LogSettings::m_ErrorLogger)		{ DME::Log::GetClientLog()->error(__VA_ARGS__); } }
#define DME_WARNING(...)			if (DME::LogSettings::m_GlobalLogger) { if (DME::LogSettings::m_WarningLogger)		{ DME::Log::GetClientLog()->warn(__VA_ARGS__); } }
#define DME_INFO(...)				if (DME::LogSettings::m_GlobalLogger) { if (DME::LogSettings::m_InfoLogger)			{ DME::Log::GetClientLog()->info(__VA_ARGS__); } }
#define DME_TRACE(...)				if (DME::LogSettings::m_GlobalLogger) { if (DME::LogSettings::m_TraceLogger)		{ DME::Log::GetClientLog()->trace(__VA_ARGS__); } }
