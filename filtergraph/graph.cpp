//@	{
//@	 "targets":[{"name":"graph.o", "type":"object"}]
//@	}

#include "./graph.hpp"

#include "utils/graphutils.hpp"

#include <map>

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
	r_input_node  = &m_nodes.find(InputNodeId)->second;
	r_output_node = &m_nodes.find(OutputNodeId)->second;
	r_input =
	    &dynamic_cast<ImageProcessorWrapper<LayerInput>*>(&r_input_node->processor())->processor();
	r_output =
	    &dynamic_cast<ImageProcessorWrapper<ImageSink>*>(&r_output_node->processor())->processor();
}

Texpainter::FilterGraph::ValidationResult Texpainter::FilterGraph::validate(Graph const& g)
{
	ValidationResult result{ValidationResult::NoError};
	processGraphNodeRecursive(
	    [&result]<class Tag>(auto const& node, Tag) {
		    if constexpr(Tag::value == GraphProcessingEvent::LoopDetected)
		    {
			    result = ValidationResult::CyclicConnections;
			    return GraphProcessing::Stop;
		    }
		    else if constexpr(Tag::value == GraphProcessingEvent::ProcessNode)
		    {
			    if(!isConnected(node))
			    {
				    result = ValidationResult::InputsNotConnected;
				    return GraphProcessing::Stop;
			    }
			    return GraphProcessing::Continue;
		    }
	    },
	    *g.node(Graph::OutputNodeId));
	return result;
}