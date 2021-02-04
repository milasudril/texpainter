//@	{
//@	 "targets":[{"name":"port_info.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_PORTINFO_HPP
#define TEXPAINTER_FILTERGRAPH_PORTINFO_HPP

#include "./port_type.hpp"

#define JSON_USE_IMPLICIT_CONVERSIONS 0
#include <nlohmann/json.hpp>

#include <cstdint>
#include <cstring>
#include <limits>

namespace Texpainter::FilterGraph
{
	enum class PortDirection : int
	{
		Input,
		Output
	};

	template<PortDirection dir>
	class PortIndex
	{
	public:
		explicit constexpr PortIndex(uint32_t val = std::numeric_limits<uint32_t>::max()): m_value{val} {}

		constexpr uint32_t value() const { return m_value; }

	private:
		uint32_t m_value;
	};

	template<PortDirection dir>
	constexpr PortIndex<dir> InvalidPortIndex{};

	template<PortDirection dir>
	inline void to_json(nlohmann::json& obj, PortIndex<dir> index)
	{
		obj = index.value();
	}

	template<PortDirection dir>
	inline void from_json(nlohmann::json const& obj, PortIndex<dir>& index)
	{
		index = PortIndex<dir>{obj.get<uint32_t>()};
	}

	template<PortDirection dir>
	constexpr bool operator==(PortIndex<dir> a, PortIndex<dir> b)
	{
		return a.value() == b.value();
	}

	template<PortDirection dir>
	constexpr bool operator!=(PortIndex<dir> a, PortIndex<dir> b)
	{
		return !(a == b);
	}

	using InputPortIndex  = PortIndex<PortDirection::Input>;
	using OutputPortIndex = PortIndex<PortDirection::Output>;
	constexpr auto InputPortNotConnected = InvalidPortIndex<PortDirection::Input>;
	constexpr auto OutputPortNotConnected = InvalidPortIndex<PortDirection::Output>;

	struct PortInfo
	{
		PortType type;
		char const* name;
	};

	constexpr bool operator==(PortInfo a, PortInfo b)
	{
		if(a.type == b.type) { return strcmp(a.name, b.name) == 0; }
		return false;
	}

	constexpr bool operator!=(PortInfo a, PortInfo b) { return !(a == b); }
}

#endif