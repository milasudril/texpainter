//@	{
//@	 "targets":[{"name":"graph.o", "type":"object"}]
//@	}

#include "./graph.hpp"

#include <stack>
#include <map>

namespace
{
	auto map_objects_to_node_id(Texpainter::FilterGraph::Graph const& g)
	{
		std::map<Texpainter::FilterGraph::Node const*, Texpainter::FilterGraph::NodeId> ret;
		std::ranges::transform(g.nodes(), std::inserter(ret, std::end(ret)), [](auto const& item) {
			return std::make_pair(&item.second, item.first);
		});
		return ret;
	}

	auto copy_nodes(Texpainter::FilterGraph::Graph const& g)
	{
		std::map<Texpainter::FilterGraph::NodeId, Texpainter::FilterGraph::Node> ret;
		std::ranges::for_each(g.nodes(), [&ret](auto const& item) {
			ret.insert(std::make_pair(item.first, item.second.clonedProcessor()));
		});
		return ret;
	}

	void copy_connections(Texpainter::FilterGraph::Graph const& g1,
	                      Texpainter::FilterGraph::Graph& g2)
	{
		std::ranges::for_each(
		    g1.nodes(), [&g2, ptr_to_id = map_objects_to_node_id(g1)](auto const& item) {
			    std::ranges::for_each(
			        item.second.inputs(),
			        [&g2, &ptr_to_id, id_a = item.first, k = 0u](auto const& conn) mutable {
				        if(conn.valid())
				        {
					        auto id_b = ptr_to_id.find(&conn.processor())->second;
					        g2.connect(
					            id_a, Texpainter::FilterGraph::InputPort{k}, id_b, conn.port());
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
	    &dynamic_cast<ImageProcessorWrapper<ImageSource<RgbaValue>>*>(&r_input_node->processor())
	         ->processor();
	r_output =
	    &dynamic_cast<ImageProcessorWrapper<ImageSink>*>(&r_output_node->processor())->processor();
}

Texpainter::FilterGraph::ValidationResult Texpainter::FilterGraph::validate(Graph const& g)
{
	enum class State : int
	{
		Init,
		InProgress,
		Done
	};

	std::stack<Node const*> nodes;
	std::map<Node const*, State> visited;

	std::ranges::for_each(g.nodes(), [&visited](auto const& item) {
		visited.insert(std::make_pair(&item.second, State::Init));
	});

	nodes.push(g.node(Graph::OutputNodeId));
	size_t peak_depth = 0;
	while(!nodes.empty())
	{
		auto node  = nodes.top();
		peak_depth = std::max(peak_depth, nodes.size());

		if(peak_depth > 32) { return ValidationResult::GraphTooDeep; }

		switch(visited[node])
		{
			case State::Init:
				visited[node] = State::InProgress;
				if(!isConnected(*node)) { return ValidationResult::InputsNotConnected; }

				for(auto const& item: node->inputs())
				{
					switch(visited[&item.processor()])
					{
						case State::Init: nodes.push(&item.processor()); break;

						case State::InProgress: return ValidationResult::CyclicConnections;

						case State::Done: break;
					}
				};
				break;

			case State::InProgress:
				visited[node] = State::Done;
				nodes.pop();
				break;

			case State::Done: nodes.pop(); break;
		}
	}

	return Texpainter::FilterGraph::ValidationResult::NoError;
}