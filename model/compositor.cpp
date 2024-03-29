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

void Texpainter::Model::Compositor::compile() const

{
	std::vector<Task> nodes;
	nodes.reserve(m_graph.size());
	using Node = FilterGraph::Node;
	std::map<Node const*, size_t> node_to_task_id;

	auto add_task = [&nodes, &node_to_task_id, task_id = static_cast<size_t>(0)](auto const& node,
	                                                                             auto) mutable {
		nodes.push_back(Task{std::ref(node), task_id, {}, std::size(node.inputs())});
		node_to_task_id.insert(std::pair{&node, task_id});
		++task_id;
		return GraphProcessing::Continue;
	};

	if(isConnectedDeep(*r_output_node)) { processGraphNodeRecursive(add_task, *r_output_node); }

	std::ranges::for_each(nodes, [&node_to_task_id](auto& item) {
		auto const inputs = item.node.get().inputs();
		for(size_t k = 0; k < std::size(inputs); ++k)
		{
			item.dependent_tasks[k] = node_to_task_id.find(&inputs[k].processor())->second;
		}
	});
	m_node_array  = std::move(nodes);
	m_node_status = std::vector<std::atomic<bool>>(std::size(m_node_array));
}

Texpainter::Model::CompositorOutput Texpainter::Model::Compositor::process(Size2d canvas_size,
                                                                           uint32_t scale) const
{
	assert(valid());
	if(m_node_array.size() == 0) [[unlikely]]
		{
			compile();
		}

	std::list<Task> task_list{std::begin(m_node_array), std::end(m_node_array)};
	std::ranges::fill(m_node_status, false);

	auto i = std::begin(task_list);
	Sched::Event e;
	auto wrap_iterator = [&task_list, &i, &e]() {
		if(i == std::end(task_list))
		{
			i = std::begin(task_list);
			e.waitAndReset();
		}
	};

	auto task_can_run = [&node_status = std::as_const(m_node_status)](auto const& task) {
		auto ret =
		    std::all_of(std::begin(task.dependent_tasks),
		                std::begin(task.dependent_tasks) + task.input_count,
		                [&node_status](auto task_id) { return node_status[task_id] == true; });
		return ret;
	};

	Sched::SignalingCounter<size_t> num_running_tasks;
	auto const domain_size = Size2d{canvas_size.width() * scale, canvas_size.height() * scale};
	while(!task_list.empty())
	{
		if(task_can_run(*i))
		{
			m_workers.addTask([item = *i,
			                   domain_size,
			                   resolution = static_cast<double>(scale),
			                   counter    = std::unique_lock{num_running_tasks},
			                   at_exit    = ScopeExitHandler{
                                   [&e, &node_status = m_node_status, task_id = i->task_id]() {
                                       node_status[task_id] = true;
                                       e.set();
                                   }}]() {
				if(!isUpToDate(item.node, domain_size))
				{
					_mm_setcsr(_mm_getcsr() | 0x8040);  // Denormals are zero
					item.node(domain_size, resolution);
				}
			});
			i = task_list.erase(i);
		}
		else
		{
			++i;
		}
		wrap_iterator();
	}
	m_current_size = domain_size;
	num_running_tasks.waitAndReset(0);
	return CompositorOutput{canvas_size, r_output_node->result()[0], scale};
}
