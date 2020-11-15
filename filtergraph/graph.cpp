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

Texpainter::PixelStore::Image Texpainter::FilterGraph::Graph::process(Input const& input,
                                                                      bool force_update) const
{
	assert(valid());
	if(m_node_array.size() == 0) [[unlikely]]
		{
			std::vector<NodeState> nodes;
			nodes.reserve(size());
			processGraphNodeRecursive(
			    [&nodes](auto const& node, auto) {
				    nodes.push_back(NodeState{std::cref(node),
				                              std::make_unique<Sched::SignalingCounter<size_t>>()});

				    // NOTE: This works because we are visiting nodes in topological order. Thus, it is
				    //       known that all sources have been added to nodes.
				    auto& item = nodes.back();
				    std::ranges::for_each(
				        item.node.get().inputs(),
				        [&nodes, counter = item.counter.get()](auto& ref) {
					        auto i = std::ranges::find_if(
					            nodes, [&value = ref.processor()](auto const& item) {
						            return &item.node.get() == &value;
					            });
					        assert(i != std::end(nodes));
					        i->signal_counters[ref.port().value()].push_back(counter);
				        });

				    return GraphProcessing::Continue;
			    },
			    *r_output_node);
			m_node_array = std::move(nodes);
		}

	r_input->pixels(input.pixels()).palette(input.palette());
	if(force_update) { r_input_node->forceUpdate(); }

	PixelStore::Image ret{input.pixels().size()};
	r_output->sink(ret.pixels());
	// NOTE: Since OutputNode does not use the internal image cache (it has no outputs)
	//       and it may happen that it is not connected to the input node, we must always
	//       recompute the output node. Otherwise, the contents of ret will be undefined,
	//       in case we already have computed the output result.
	r_output_node->forceUpdate();

	std::ranges::for_each(m_node_array, [size = input.pixels().size()](auto& item) {
		item.counter->waitAndReset(item.node.get().inputPorts().size());
		item.node(size);
		std::ranges::for_each(item.signal_counters, [](auto& counter) {
			std::ranges::for_each(counter, [](auto value) { ++(*value); });
		});
	});
	return ret;
}