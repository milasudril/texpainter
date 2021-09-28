//@	{
//@	  "targets":[{"name":"compositor_new.o", "type":"object"}]
//@	}

#include "./compositor_new.hpp"

#include "utils/graphutils.hpp"

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

void Texpainter::Model::Compositor::process(Span2d<PixelStore::Pixel> canvas, double) const
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

#if 0
	std::list<Task> task_list;
	std::ranges::transform(m_node_array,
	                       std::back_inserter(task_list),
	                       [index = static_cast<size_t>(0)](auto item) mutable {
		                       return Task{item};
	                       });

	std::vector<std::atomic<bool>> status(std::size(task_list));

	auto i = std::begin(task_list);
	Sched::Event e;
	auto wrap_iterator = [&task_list, &i, &e]() {
		if(i == std::end(task_list))
		{
			i = std::begin(task_list);
			e.waitAndReset();
		}
	};

	Sched::SignalingCounter<size_t> counter;
	size_t num_tasks = 0;
	while(!task_list.empty())
	{
		if(i->blocked())
		{
			++i;
			wrap_iterator();
		}
		else
		{
			++num_tasks;
			workers.addTask([item = std::move(*i),
			                 resolution,
			                 &status,
			                 counter = std::unique_lock{counter},
			                 at_exit = ScopeExitHandler{[&e]() { e.set(); }}]() {
				if(isConnected(item.node())
				{
					_mm_setcsr(_mm_getcsr() | 0x8040);  // Denormals are zero
					item.node(size, resolution);
				}
				status[item.index()] = true;
			});
			i = task_list.erase(i);
			wrap_iterator();
		}
	}
	counter.waitAndReset(0);
#endif

#if 0

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
