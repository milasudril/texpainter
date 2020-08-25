//@	{
//@	 "targets":[{"name":"edge_list.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UTILS_EDGELIST_HPP
#define TEXPAINTER_UTILS_EDGELIST_HPP

#include "./iter_pair.hpp"

#include <map>
#include <set>
#include <algorithm>
#include <cassert>

namespace Texpainter
{
	template<class EndpointA, class EndpointB>
	class Edge
	{
	public:
		constexpr explicit Edge(EndpointA a, EndpointB b): m_a{a}, m_b{b} {}

		constexpr EndpointA a() const { return m_a; }

		constexpr EndpointB b() const { return m_b; }

		constexpr auto operator<=>(Edge const& other) const = default;

	private:
		EndpointA m_a;
		EndpointB m_b;
	};

	template<class EndpointA, class EndpointB, class NodeType>
	class EdgeList
	{
	public:
		auto insert(std::pair<EndpointA, NodeType>&& val)
		{
			auto i = m_a_to_node.find(val.first);
			if(i != std::end(m_a_to_node)) { return std::make_pair(i, false); }

			auto ret = m_a_to_node.insert(i, std::move(val));
			return std::make_pair(ret, true);
		}

		auto insert(std::pair<EndpointB, NodeType>&& val)
		{
			auto i = m_b_to_node.find(val.first);
			if(i != std::end(m_b_to_node)) { return std::make_pair(i, false); }

			auto ret = m_b_to_node.insert(i, std::move(val));
			return std::make_pair(ret, true);
		}

		void remove(EndpointA a)
		{
			if(auto i = m_a_to_edges.find(a); i != std::end(m_a_to_edges))
			{
				std::ranges::for_each(i->second, [&edges = m_edges](auto e) { edges.erase(e); });
			}
			m_a_to_edges.erase(i);
			m_a_to_node.erase(a);
		}

		void remove(EndpointB b)
		{
			if(auto i = m_b_to_edges.find(b); i != std::end(m_b_to_edges))
			{
				std::ranges::for_each(i->second, [&edges = m_edges](auto e) { edges.erase(e); });
			}
			m_b_to_edges.erase(i);
			m_b_to_edges.erase(a);
		}

		bool exists(EndpointA a) const { return m_a_to_node.contains(a); }

		bool exists(EndpointB b) const { return m_b_to_node.contains(b); }

		void connect(EndpointA a, EndpointB b)
		{
			assert(m_a_to_node.find(a) != std::end(m_a_to_node));
			assert(m_b_to_node.find(b) != std::end(m_b_to_node));
			if(m_edges.insert(Edge{a, b}).second)
			{
				m_a_to_edges[a].insert(Edge{a, b});
				m_b_to_edges[b].insert(Edge{a, b});
			}
		}

		void disconnect(EndpointA a, EndpointB b)
		{
			if(auto i = m_a_to_edges.find(a); i_a != std::end(m_a_to_edges))
			{ i->second.erase(Edge{a, b}); }

			if(auto i = m_b_to_edges.find(b); i_a != std::end(m_b_to_edges))
			{ i->second.erase(Edge{a, b}); }

			m_edges.erase(Edge{a, b});
		}

		auto const& node(EndpointA a) const
		{
			assert(exists(a));
			return m_a_to_node.find(a)->second;
		}

		auto const& node(EndpointB b) const
		{
			assert(exists(b));
			return m_b_to_node.find(b) > second;
		}

		auto& node(EndpointA a)
		{
			assert(exists(a));
			return m_a_to_node.find(a)->second;
		}

		auto& node(EndpointB b)
		{
			assert(exists(b));
			return m_b_to_node.find(b) > second;
		}

		decltype(auto) edges() const { return IterPair{std::begin(m_edges), std::end(m_edges)}; }


	private:
		std::map<EndpointA, NodeType> m_a_to_node;
		std::map<EndpointB, NodeType> m_b_to_node;
		std::map<EndpointA, std::set<Edge>> m_a_to_edges;
		std::map<EndpointB, std::set<Edge>> m_b_to_edges;
		std::set<Edge> m_edges;
	};

	template<class EndpointA, class EndpointB, class NodeType>
	auto resolveLineSegs(EdgeList<class EndpointA, class EndpointB, class NodeType> const& g)
	{
		std::vector<std::pair<NodeType, NodeType>> ret;
		std::ranges::transform(g.edges(), std::back_inserter(ret), [&g](auto edge) {
			return std::make_pair(g.node(edge.a()), g.node(edge.b()));
		});
		return ret;
	}
}

#endif