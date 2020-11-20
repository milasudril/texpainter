//@	{
//@	 "targets":[{"name":"graph.o", "type":"object"}]
//@	}

#include "./graph.hpp"

namespace
{
	auto map_objects_to_node_id(Texpainter::FilterGraph::Graph const& g)
	{
		std::map<Texpainter::FilterGraph::Node const*, Texpainter::FilterGraph::NodeId> ret;
		std::ranges::transform(
		    g.nodesWithId(), std::inserter(ret, std::end(ret)), [](auto const& item) {
			    return std::make_pair(&item.second, item.first);
		    });
		return ret;
	}

	auto copy_nodes(Texpainter::FilterGraph::Graph const& g)
	{
		std::map<Texpainter::FilterGraph::NodeId, Texpainter::FilterGraph::Node> ret;
		std::ranges::for_each(g.nodesWithId(), [&ret](auto const& item) {
			ret.insert(std::make_pair(item.first, item.second.clonedProcessor()));
		});
		return ret;
	}

	void copy_connections(Texpainter::FilterGraph::Graph const& g1,
	                      Texpainter::FilterGraph::Graph& g2)
	{
		std::ranges::for_each(
		    g1.nodesWithId(), [&g2, ptr_to_id = map_objects_to_node_id(g1)](auto const& item) {
			    std::ranges::for_each(
			        item.second.inputs(),
			        [&g2, &ptr_to_id, id_a = item.first, k = 0u](auto const& conn) mutable {
				        if(conn.valid())
				        {
					        auto id_b = ptr_to_id.find(&conn.processor())->second;
					        g2.connect(id_a,
					                   Texpainter::FilterGraph::InputPortIndex{k},
					                   id_b,
					                   conn.port());
				        }
				        ++k;
			        });
		    });
	}
}

Texpainter::FilterGraph::Graph::Graph(Graph const& other)
    : m_nodes{copy_nodes(other)}
    , m_current_id{other.m_current_id}
{
	copy_connections(other, *this);
}