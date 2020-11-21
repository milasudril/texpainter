//@	{
//@	  "targets":[{"name":"compositor.o", "type":"object"}]
//@	}

#include "./compositor.hpp"

#include "utils/graphutils.hpp"

Texpainter::FilterGraph::ValidationResult Texpainter::Model::validate(Compositor const& g)
{
	using FilterGraph::ValidationResult;

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
	    *g.node(Compositor::OutputNodeId));
	return result;
}

void Texpainter::Model::Compositor::process(Span2d<PixelStore::Pixel> canvas) const
{
	assert(valid());
	if(m_node_array.size() == 0) [[unlikely]]
		{
			std::vector<NodeState> nodes;
			nodes.reserve(m_graph.size());
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

	r_output->sink(canvas);

	// NOTE: Since OutputNode does not use the internal image cache (it has no outputs)
	//       and it may happen that it is not connected to the input node, we must always
	//       recompute the output node. Otherwise, the contents of ret will be undefined,
	//       in case we already have computed the output result.
	r_output_node->forceUpdate();

	Sched::SignalingCounter<size_t> task_counter;

	auto schedule_task = [&workers = m_workers, size = canvas.size(), &task_counter](auto& item) {
		workers.addTask([&item, size, &task_counter]() {
			item.counter->waitAndReset(item.node.get().inputPorts().size());
			item.node(size);
			std::ranges::for_each(item.signal_counters, [](auto& counter) {
				std::ranges::for_each(counter, [](auto value) { ++(*value); });
			});
			++task_counter;
		});
	};

	std::ranges::for_each(m_node_array, schedule_task);
	task_counter.waitAndReset(m_node_array.size());
}