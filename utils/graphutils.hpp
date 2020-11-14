//@	{
//@	  "targets":[{"name":"graphutils.hpp","type":"include"}]
//@	 }

#ifndef TEXPAINTER_UTILS_GRAPHUTILS_HPP
#define TEXPAINTER_UTILS_GRAPHUTILS_HPP

#include <vector>
#include <cstddef>
#include <stack>
#include <stdexcept>
#include <type_traits>

namespace Texpainter
{
	namespace graphutils_detail
	{
		enum class Mark : int
		{
			Init,
			InProgress,
			Done
		};

		template<class T>
		inline bool valid(T x, std::enable_if_t<std::is_integral_v<T>, int> = 0)
		{
			return x != static_cast<T>(-1);
		}

		template<class ItemCallback, class Graph, class Node>
		void processGraphNodeRecursive(ItemCallback&& cb,
		                               Graph const& graph,
		                               Node const& node,
		                               std::stack<Node const*>& nodes_to_visit,
		                               std::vector<Mark>& visited)
		{
			switch(visited[static_cast<size_t>(id(node))])
			{
				case Mark::Init:
					nodes_to_visit.push(&node);
					while(!nodes_to_visit.empty())
					{
						auto node    = nodes_to_visit.top();
						auto node_id = id(*node);
						switch(visited[static_cast<size_t>(node_id)])
						{
							case Mark::Init:
							{
								auto processEdge = [&graph, &nodes_to_visit, &visited](
								                       auto const& edge) {
									auto node_id = reference(edge);
									if(valid(node_id))
									{
										switch(visited[static_cast<size_t>(node_id)])
										{
											case Mark::Init:
												nodes_to_visit.push(&getNodeById(graph, node_id));
												break;

											case Mark::InProgress:
												throw std::runtime_error{
												    "Cyclic dependency detected"};

											case Mark::Done: break;
										}
									}
								};
								visited[static_cast<size_t>(node_id)] = Mark::InProgress;
								visitEdges(std::move(processEdge), *node);
								break;
							}
							case Mark::InProgress:
								visited[static_cast<size_t>(node_id)] = Mark::Done;
								cb(*node);
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

	template<class ItemCallback, class Graph, class Node>
	void processGraphNodeRecursive(ItemCallback&& cb, Graph const& graph, Node const& node)
	{
		using std::size;
		auto const N = size(graph);
		std::vector<graphutils_detail::Mark> visited(N);
		std::stack<typename Graph::node_type const*> nodes_to_visit;
		graphutils_detail::processGraphNodeRecursive(cb, graph, node, nodes_to_visit, visited);
	}

	template<class ItemCallback, class Graph>
	void visitNodesInTopoOrder(ItemCallback&& cb, Graph const& graph)
	{
		using std::size;
		auto const N = size(graph);
		std::vector<graphutils_detail::Mark> visited(N);
		std::stack<typename Graph::node_type const*> nodes_to_visit;

		auto processNode = [&cb, &graph, &nodes_to_visit, &visited](auto const& node) {
			graphutils_detail::processGraphNodeRecursive(cb, graph, node, nodes_to_visit, visited);
		};

		visitNodes(std::move(processNode), graph);
	}
}

#endif