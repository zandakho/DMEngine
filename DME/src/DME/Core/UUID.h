#pragma once

#include <string>
#include <format>

namespace DME {

	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;

		operator uint64_t() const { return m_UUID; }

		uint64_t GetUUID() const { return m_UUID; }
		std::string PrintUUIDAsString()
		{
			std::string id = std::format("{0}", GetUUID());
			return id;
		}

	private:
		uint64_t m_UUID;
	};

}

namespace std {

	template <typename T> struct hash;

	template<>
	struct hash<DME::UUID>
	{
		std::size_t operator()(const DME::UUID& uuid) const
		{
			return static_cast<uint64_t>(uuid);
		}
	};

}