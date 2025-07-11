#pragma once

#include "DME/Core/Base.h"

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


	};
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
