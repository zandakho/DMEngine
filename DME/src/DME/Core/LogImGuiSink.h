#pragma once
#include <spdlog/sinks/base_sink.h>
#include <mutex>
#include <deque>
#include <string>

namespace DME
{
	struct ImGuiLogEntry
	{
		spdlog::level::level_enum level;
		std::string message;
	};

	class LogImGuiSink : public spdlog::sinks::base_sink<std::mutex>
	{
	public:
		LogImGuiSink(size_t maxBuffer = 2000)
			: m_MaxBuffer(maxBuffer)
		{
		}

		const std::deque<ImGuiLogEntry>& GetBuffer() const { return m_Buffer; }

	protected:
		void sink_it_(const spdlog::details::log_msg& msg) override
		{
			spdlog::memory_buf_t formatted;
			formatter_->format(msg, formatted);

			if (m_Buffer.size() >= m_MaxBuffer)
				m_Buffer.pop_front();

			m_Buffer.push_back({ msg.level, fmt::to_string(formatted) });
		}

		void flush_() override {}

	private:
		std::deque<ImGuiLogEntry> m_Buffer;
		size_t m_MaxBuffer;
	};

}
