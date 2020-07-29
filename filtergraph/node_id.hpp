//@	{
//@	 "targets":[{"name":"node_id.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_NODEID_HPP
#define TEXPAINTER_FILTERGRAPH_NODEID_HPP

#include "./processor_node.hpp"
#include "./node_id.hpp"

#include <compare>

namespace Texpainter::FilterGraph
{
	class NodeId
	{
	public:
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

	private:
		uint64_t m_value;
	};
}

#endif