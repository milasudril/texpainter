//@	{
//@	  "targets":[{"name":"graphutils.hpp","type":"include"}]
//@	 }

#ifndef TEXPAINTER_UTILS_GRAPHUTILS_HPP
#define TEXPAINTER_UTILS_GRAPHUTILS_HPP

#include <map>
#include <cstddef>
#include <stack>
#include <stdexcept>
#include <type_traits>

namespace Texpainter
{
	enum class GraphProcessing : int
	{
		Continue,
		Stop
	};

	enum class GraphProcessingEvent : int
	{
		ProcessNode,
		LoopDetected
	};

	template<GraphProcessingEvent e>
	using GraphutilsEvent = std::integral_constant<GraphProcessingEvent, e>;

	using GraphutilsProcessNode = GraphutilsEvent<GraphProcessingEvent::ProcessNode>;

	using GraphutilsLoopDetected = GraphutilsEvent<GraphProcessingEvent::LoopDetected>;

	namespace graphutils_detail
	{
		enum class Mark : int
		{
			Init,
			InProgress,
			Done
		};

		template<class ItemCallback, class Node>
		void processGraphNodeRecursive(ItemCallback&& cb,
		                               Node const& node,
		                               std::stack<Node const*>& nodes_to_visit,
		                               std::map<Node const*, Mark>& visited)
		{
			switch(visited[&node])
			{
				case Mark::Init:
					nodes_to_visit.push(&node);
					while(!nodes_to_visit.empty())
					{
						auto node_next = nodes_to_visit.top();
						switch(visited[node_next])
						{
							case Mark::Init:
							{
								auto processEdge =
								    [&nodes_to_visit, &visited, &cb](auto const& edge) {
									    Node const* other = reference(edge);
									    if(other != nullptr)
									    {
										    switch(visited[other])
										    {
											    case Mark::Init:
												    nodes_to_visit.push(other);
												    return GraphProcessing::Continue;

											    case Mark::InProgress:
												    if(cb(*other, GraphutilsLoopDetected{})
												       == GraphProcessing::Stop)
												    { return GraphProcessing::Stop; }
												    break;

											    case Mark::Done: return GraphProcessing::Continue;
										    }
									    }
									    return GraphProcessing::Continue;
								    };
								visited[node_next] = Mark::InProgress;
								visitEdges(std::move(processEdge), *node_next);
								break;
							}
							case Mark::InProgress:
								visited[node_next] = Mark::Done;
								if(cb(*node_next, GraphutilsProcessNode{}) == GraphProcessing::Stop)
								{ return; }
								nodes_to_visit.pop();
								break;
							default: nodes_to_visit.pop(); break;
						}
					}
					break;
				case Mark::InProgress: throw std::runtime_error{"Cyclic dependency detected"};
				case Mark::Done: return;
			}
		}
	}

	template<class ItemCallback, class Node>
	void processGraphNodeRecursive(ItemCallback&& cb, Node const& node)
	{
		std::map<Node const*, graphutils_detail::Mark> visited;
		std::stack<Node const*> nodes_to_visit;
		graphutils_detail::processGraphNodeRecursive(cb, node, nodes_to_visit, visited);
	}

	template<class ItemCallback, class Graph>
	void visitNodesInTopoOrder(ItemCallback&& cb, Graph const& graph)
	{
		using std::size;
		std::map<typename Graph::node_type const*, graphutils_detail::Mark> visited;
		std::stack<typename Graph::node_type const*> nodes_to_visit;

		auto processNode = [&cb, &nodes_to_visit, &visited](auto const& node) {
			graphutils_detail::processGraphNodeRecursive(cb, node, nodes_to_visit, visited);
		};

		visitNodes(std::move(processNode), graph);
	}
}

#endif