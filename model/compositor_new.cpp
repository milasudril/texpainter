//@	{
//@	  "targets":[{"name":"compositor_new.o", "type":"object"}]
//@	}

#include "./compositor_new.hpp"

#include "utils/graphutils.hpp"

#include <limits>
#include <xmmintrin.h>

Texpainter::FilterGraph::ValidationResult Texpainter::Model::validate(Compositor const& g)
{
	using FilterGraph::ValidationResult;

	ValidationResult result{ValidationResult::NoError};
	processGraphNodeRecursive(
	    [&result]<class Tag>(auto const&, Tag) {
		    if constexpr(Tag::value == GraphProcessingEvent::LoopDetected)
		    {
			    result = ValidationResult::CyclicConnections;
			    return GraphProcessing::Stop;
		    }
			return GraphProcessing::Continue;
	    },
	    *g.node(Compositor::OutputNodeId));
	return result;
}

void Texpainter::Model::Compositor::process(Span2d<PixelStore::Pixel>,
                                            double) const
{
	assert(valid());
	if(m_node_array.size() == 0) [[unlikely]]
		{
			std::vector<std::reference_wrapper<FilterGraph::Node const>> nodes;
			nodes.reserve(m_graph.size());
			processGraphNodeRecursive(
			    [&nodes](auto const& node, auto) {
				    nodes.push_back(std::ref(node));
				    return GraphProcessing::Continue;
			    },
			    *r_output_node);
			m_node_array = std::move(nodes);
		}
#if 0

	r_output->sink(canvas);

	// NOTE: Since OutputNode does not use the internal image cache (it has no outputs)
	//       and it may happen that it is not connected to the input node, we must always
	//       recompute the output node. Otherwise, the contents of ret will be undefined,
	//       in case we already have computed the output result.
	r_output_node->forceUpdate();

	Sched::SignalingCounter<size_t> task_counter;

	auto schedule_task =
	    [&workers = m_workers, size = canvas.size(), resolution, &task_counter](auto& item) {
		    workers.addTask([&item, size, resolution, &task_counter]() {
			    item.counter->waitAndReset(item.node.get().inputPorts().size());
			    _mm_setcsr(_mm_getcsr() | 0x8040);  // Denormals are zero
			    item.node(size, resolution);
			    std::ranges::for_each(item.signal_counters, [](auto& counter) {
				    std::ranges::for_each(counter, [](auto value) { ++(*value); });
			    });
			    ++task_counter;
		    });
	    };

	std::ranges::for_each(m_node_array, schedule_task);
	task_counter.waitAndReset(m_node_array.size());
#endif
}
