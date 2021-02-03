//@	{
//@	 "targets":[{"name":"node_id.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_NODEID_HPP
#define TEXPAINTER_FILTERGRAPH_NODEID_HPP

#include "utils/to_string.hpp"

#define JSON_USE_IMPLICIT_CONVERSIONS 0
#include <nlohmann/json.hpp>

#include <compare>
#include <map>
#include <limits>

namespace Texpainter::FilterGraph
{
	class NodeId
	{
	public:
		constexpr explicit NodeId(uint64_t val): m_value{val} {}

		constexpr NodeId(): m_value{0} {}

		constexpr uint64_t value() const { return m_value; }

		constexpr NodeId& operator++()
		{
			++m_value;
			return *this;
		}

		constexpr NodeId operator++(int)
		{
			auto current = *this;
			++(*this);
			return current;
		}

		constexpr auto operator<=>(NodeId const& other) const = default;

		constexpr explicit operator size_t() const { return m_value; }

	private:
		uint64_t m_value;
	};

	inline void to_json(nlohmann::json& obj, NodeId id) { obj = id.value(); }

	inline void from_json(nlohmann::json const& obj, NodeId& id)
	{
		id = NodeId{obj.get<uint64_t>()};
	}

	inline auto toString(NodeId id) { return Texpainter::toString(id.value()); }

	constexpr auto InvalidNodeId = NodeId{std::numeric_limits<uint64_t>::max()};
}

namespace nlohmann
{
	template<class T>
	struct adl_serializer<std::map<Texpainter::FilterGraph::NodeId, T>>
	{
		static void to_json(json& j, std::map<Texpainter::FilterGraph::NodeId, T> const& vals)
		{
			for(auto const& elem: vals)
			{
				j[toString(elem.first)] = elem.second;
			}
		}

		static void from_json(const json& j, std::map<Texpainter::FilterGraph::NodeId, T>& ret)
		{
			for(auto it = std::begin(j); it != std::end(j); ++it)
			{
				adl_serializer<T>::from_json(
				    it.value(), ret[Texpainter::FilterGraph::NodeId{std::stoull(it.key())}]);
			}
		}
	};
}

#endif