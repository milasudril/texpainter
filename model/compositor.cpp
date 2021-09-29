//@	{
//@	  "targets":[{"name":"compositor.o", "type":"object"}]
//@	}

#include "./compositor.hpp"

#include "utils/graphutils.hpp"
#include "utils/scope_exit_handler.hpp"

#include "sched/event.hpp"
#include "sched/signaling_counter.hpp"

#include <list>
#include <xmmintrin.h>  // _mm_setcsr

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

void Texpainter::Model::Compositor::process(Span2d<PixelStore::Pixel> canvas,
                                            double resolution) const
{
	assert(valid());
	if(m_node_array.size() == 0) [[unlikely]]
		{
			// TODO: prune disconnected branches
			std::vector<Task> nodes;
			nodes.reserve(m_graph.size());
			processGraphNodeRecursive(
			    [&nodes](auto const& node, auto) {
				    nodes.push_back(Task{std::ref(node)});
				    return GraphProcessing::Continue;
			    },
			    *r_output_node);
			m_node_array = std::move(nodes);
		}

	r_output->sink(canvas);

	std::list<Task> task_list{std::begin(m_node_array), std::end(m_node_array)};

	auto i = std::begin(task_list);
	Sched::Event e;
	auto wrap_iterator = [&task_list, &i, &e]() {
		if(i == std::end(task_list))
		{
			puts("====================");
			i = std::begin(task_list);
			e.waitAndReset();
		}
	};

	Sched::SignalingCounter<size_t> num_running_tasks;
	while(!task_list.empty())
	{
		if(inputsUpToDate(i->node))
		{
			m_workers.addTask([item = std::move(*i),
			                   size = canvas.size(),
			                   resolution,
			                   counter = std::unique_lock{num_running_tasks},
			                   at_exit = ScopeExitHandler{[&e]() { e.set(); }}]() {
				if(!isUpToDate(item.node))
				{
					_mm_setcsr(_mm_getcsr() | 0x8040);  // Denormals are zero
					item.node(size, resolution);
				}
			});
			i = task_list.erase(i);
		}
		else
		{
			printf("%zu is blocked\n", i->node.get().nodeId().value());
			++i;
		}
		wrap_iterator();
	}
	num_running_tasks.waitAndReset(0);
}
