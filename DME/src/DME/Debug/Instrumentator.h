#pragma once


// --- Dependencies --------------------------------------------//
#include "DME/Core/Log.h"										//
// -------------------------------------------------------------//

// --- Dependencies --------------------------------------------//
#include <string>												//
#include <chrono>												//
#include <algorithm>											//
#include <fstream>												//
#include <thread>												//
// -------------------------------------------------------------//


namespace DME
{
	struct ProfileResult
	{
		std::string Name;
		long long Start, End;
		size_t ThreadID;
	};

	struct InstrumetationSession
	{
		std::string Name;
	};

	class Instrumentator
	{
	private:
		InstrumetationSession* m_CurrentSession;
		std::ofstream m_OutputStream;
		int m_ProfileCount;
	public:
		Instrumentator()
			: m_CurrentSession(nullptr), m_ProfileCount(0)
		{
		}

		void BeginSession(const std::string& name, const std::string& filepath = "results.json")
		{
			
			m_OutputStream.open(filepath);
			WriteHeader();
			m_CurrentSession = new InstrumetationSession{ name };

		}

		void EndSession()
		{
			WriteFooter();
			m_OutputStream.close();
			delete m_CurrentSession;
			m_CurrentSession = nullptr;
			m_ProfileCount = 0;
		}

		void WriteProfile(const ProfileResult& result)
		{
			if (m_ProfileCount++ > 0)
			{
				m_OutputStream << ",";
			}

			std::string name = result.Name;
			std::replace(name.begin(), name.end(), '"', '\'');
			m_OutputStream << "{";
			m_OutputStream << "\"cat\":\"function\",";
			m_OutputStream << "\"dur\":" << (result.End - result.Start) << ",";
			m_OutputStream << "\"name\":\"" << name << "\",";
			m_OutputStream << "\"ph\":\"X\",";
			m_OutputStream << "\"pid\":0,";
			m_OutputStream << "\"tid\":" << result.ThreadID << ",";
			m_OutputStream << "\"ts\":" << result.Start;
			m_OutputStream << "}";

			m_OutputStream.flush();
		}

		void WriteHeader()
		{
			m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
			m_OutputStream.flush();
		}

		void WriteFooter()
		{
			m_OutputStream << "]}";
			m_OutputStream.flush();
		}

		static Instrumentator& Get()
		{
			static Instrumentator instance;
			return instance;
		}
	};

	class InstrumentatorTimer
	{
	public:
		InstrumentatorTimer(const char* name) : m_Name(name), m_Stopped(false)
		{
			m_StartTimepoint = std::chrono::high_resolution_clock::now();
		}

		~InstrumentatorTimer()
		{
			if (!m_Stopped)
				Stop();
		}

		void Stop()
		{
			auto endTimepoint = std::chrono::high_resolution_clock::now();

			long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
			long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

			size_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
			Instrumentator::Get().WriteProfile({ m_Name, start, end, static_cast<uint32_t>(threadID)});

			m_Stopped = true;
		}

	private:
		const char* m_Name;
		std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
		bool m_Stopped;
	};
}

#define HZ_PROFILE 1

#if HZ_PROFILE
#define CONCAT(x, y) x ## y
#define C(x, y) CONCAT(x, y)
#define HZ_PROFILE_BEGIN_SESSION(name, filepath) ::DME::Instrumentator::Get().BeginSession(name, filepath)
#define HZ_PROFILE_END_SESSION() ::DME::Instrumentator::Get().EndSession()
#define HZ_PROFILE_SCOPE(name) ::DME::InstrumentatorTimer C(timer,__LINE__)(name);
#define HZ_PROFILE_FUNCTION() HZ_PROFILE_SCOPE(__FUNCSIG__)

#else

#define CONCAT(x, y) x ## y
#define C(x, y) CONCAT(x, y)
#define HZ_PROFILE_BEGIN_SESSION(name, filepath)
#define HZ_PROFILE_END_SESSION()
#define HZ_PROFILE_SCOPE(name)
#define HZ_PROFILE_FUNCTION()

#endif