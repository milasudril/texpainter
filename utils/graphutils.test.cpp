//@	{
//@	  "targets":[{"name":"graphutils.test","type":"application", "autorun":1}]
//@	 }

#undef NDEBUG

#include "./graphutils.hpp"

#include <algorithm>
#include <cassert>
#include <vector>

namespace
{
	struct Node;

	struct Edge
	{
		Edge(Node* node): r_other{node} {}
		Node* r_other;
	};

	auto reference(Edge e) { return e.r_other; }

	struct Node
	{
		std::vector<Edge> m_edges;
	};

	template<class F>
	void visitEdges(F&& f, Node const& node)
	{
		std::for_each(std::begin(node.m_edges), std::end(node.m_edges), f);
	}

	struct Graph
	{
		using node_type = Node;
		std::vector<Node> m_nodes;

		Node& operator[](size_t index) { return m_nodes[index]; }
	};

	template<class F>
	void visitNodes(F&& f, Graph const& g)
	{
		std::for_each(std::begin(g.m_nodes), std::end(g.m_nodes), f);
	}
}

namespace Testcases
{
	void graphutilsVisitNodesDag()
	{
		Graph graph;
		graph.m_nodes = std::vector<Node>(11);

		graph[0].m_edges = std::vector<Edge>{&graph[1], &graph[2], &graph[3]};
		graph[1].m_edges = std::vector<Edge>{&graph[4], &graph[5], &graph[6]};
		graph[2].m_edges = std::vector<Edge>{&graph[4], &graph[7], &graph[8]};
		graph[3].m_edges = std::vector<Edge>{&graph[8]};
		graph[4].m_edges = std::vector<Edge>{&graph[10]};
		graph[5].m_edges = std::vector<Edge>{&graph[10]};
		graph[6].m_edges = std::vector<Edge>{&graph[10]};
		graph[7].m_edges = std::vector<Edge>{&graph[10]};
		graph[8].m_edges = std::vector<Edge>{&graph[10]};
		graph[9].m_edges = std::vector<Edge>{&graph[4], &graph[5], &graph[6]};

		std::vector<Node const*> nodes_sorted;
		Texpainter::visitNodesInTopoOrder(
		    [&nodes_sorted](Node const& node, auto) {
			    nodes_sorted.push_back(&node);
			    return Texpainter::GraphProcessing::Continue;
		    },
		    graph);

		auto index_of = [](std::vector<Node const*> const& nodes, Node const* node) {
			auto i = std::find(std::begin(nodes), std::end(nodes), node);
			return i - std::begin(nodes);
		};

		std::map<Node const*, ptrdiff_t> node_index;
		std::for_each(std::begin(graph.m_nodes),
		              std::end(graph.m_nodes),
		              [index_of, &nodes_sorted, &node_index](auto const& node) {
			              node_index[&node] = index_of(nodes_sorted, &node);
		              });

		assert(node_index[&graph[10]] == 0);
		assert(node_index[&graph[10]] < node_index[&graph[4]]);
		assert(node_index[&graph[10]] < node_index[&graph[5]]);
		assert(node_index[&graph[10]] < node_index[&graph[6]]);
		assert(node_index[&graph[10]] < node_index[&graph[7]]);
		assert(node_index[&graph[10]] < node_index[&graph[8]]);
		assert(node_index[&graph[4]] < node_index[&graph[1]]);
		assert(node_index[&graph[4]] < node_index[&graph[2]]);
		assert(node_index[&graph[4]] < node_index[&graph[9]]);
		assert(node_index[&graph[5]] < node_index[&graph[1]]);
		assert(node_index[&graph[5]] < node_index[&graph[9]]);
		assert(node_index[&graph[6]] < node_index[&graph[1]]);
		assert(node_index[&graph[6]] < node_index[&graph[9]]);
		assert(node_index[&graph[7]] < node_index[&graph[2]]);
		assert(node_index[&graph[8]] < node_index[&graph[2]]);
		assert(node_index[&graph[8]] < node_index[&graph[3]]);
		assert(node_index[&graph[1]] < node_index[&graph[0]]);
		assert(node_index[&graph[2]] < node_index[&graph[0]]);
		assert(node_index[&graph[3]] < node_index[&graph[0]]);
	}

	void graphutilsVisitNodesNotADag()
	{
		Graph graph;
		graph.m_nodes = std::vector<Node>(11);

		graph[0].m_edges  = std::vector<Edge>{&graph[1], &graph[2], &graph[3]};
		graph[1].m_edges  = std::vector<Edge>{&graph[4], &graph[5], &graph[6]};
		graph[2].m_edges  = std::vector<Edge>{&graph[4], &graph[7], &graph[8]};
		graph[3].m_edges  = std::vector<Edge>{&graph[8]};
		graph[4].m_edges  = std::vector<Edge>{&graph[10]};
		graph[5].m_edges  = std::vector<Edge>{&graph[10]};
		graph[6].m_edges  = std::vector<Edge>{&graph[10]};
		graph[7].m_edges  = std::vector<Edge>{&graph[10]};
		graph[8].m_edges  = std::vector<Edge>{&graph[10]};
		graph[9].m_edges  = std::vector<Edge>{&graph[4], &graph[5], &graph[6]};
		graph[10].m_edges = std::vector<Edge>{&graph[9]};

		auto loop_detected = false;

		Texpainter::visitNodesInTopoOrder(
		    [&loop_detected](Node const&, auto tag) {
			    if(tag.value == Texpainter::GraphProcessingEvent::LoopDetected)
			    { loop_detected = true; }
			    return Texpainter::GraphProcessing::Continue;
		    },
		    graph);

		assert(loop_detected);
	}

	void graphutilsProcessGraphNodeRecursive()
	{
		Graph graph;
		graph.m_nodes = std::vector<Node>(11);

		graph[0].m_edges = std::vector<Edge>{&graph[1], &graph[2], &graph[3]};
		graph[1].m_edges = std::vector<Edge>{&graph[4], &graph[5], &graph[6]};
		graph[2].m_edges = std::vector<Edge>{&graph[4], &graph[7], &graph[8]};
		graph[3].m_edges = std::vector<Edge>{&graph[8]};
		graph[4].m_edges = std::vector<Edge>{&graph[10]};
		graph[5].m_edges = std::vector<Edge>{&graph[10]};
		graph[6].m_edges = std::vector<Edge>{&graph[10]};
		graph[7].m_edges = std::vector<Edge>{&graph[10]};
		graph[8].m_edges = std::vector<Edge>{&graph[10]};
		graph[9].m_edges = std::vector<Edge>{&graph[4], &graph[5], &graph[6]};

		std::vector<Node const*> nodes_sorted;
		Texpainter::processGraphNodeRecursive(
		    [&nodes_sorted](Node const& node, auto) {
			    nodes_sorted.push_back(&node);
			    return Texpainter::GraphProcessing::Continue;
		    },
		    graph.m_nodes[9]);

		auto index_of = [](std::vector<Node const*> const& nodes, Node const* node) {
			auto i = std::find(std::begin(nodes), std::end(nodes), node);
			return i - std::begin(nodes);
		};

		std::map<Node const*, ptrdiff_t> node_index;
		std::for_each(std::begin(graph.m_nodes),
		              std::end(graph.m_nodes),
		              [index_of, &nodes_sorted, &node_index](auto const& node) {
			              node_index[&node] = index_of(nodes_sorted, &node);
		              });

		assert(node_index[&graph[10]] == 0);
		assert(node_index[&graph[4]] < node_index[&graph[9]]);
		assert(node_index[&graph[5]] < node_index[&graph[9]]);
		assert(node_index[&graph[6]] < node_index[&graph[9]]);
	}


	void graphutilsProcessGraphNodeRecursiveSimpleDiamond()
	{
		Graph graph;
		graph.m_nodes = std::vector<Node>(5);

		graph[0].m_edges = std::vector<Edge>{&graph[1], &graph[2], &graph[3]};
		graph[1].m_edges = std::vector<Edge>{&graph[4]};
		graph[2].m_edges = std::vector<Edge>{&graph[4]};
		graph[3].m_edges = std::vector<Edge>{&graph[4]};


		std::vector<Node const*> nodes_sorted;
		Texpainter::processGraphNodeRecursive(
		    [&nodes_sorted](Node const& node, auto) {
			    nodes_sorted.push_back(&node);
			    return Texpainter::GraphProcessing::Continue;
		    },
		    graph.m_nodes[0]);

		auto index_of = [](std::vector<Node const*> const& nodes, Node const* node) {
			auto i = std::find(std::begin(nodes), std::end(nodes), node);
			return i - std::begin(nodes);
		};

		std::map<Node const*, ptrdiff_t> node_index;
		std::for_each(std::begin(graph.m_nodes),
		              std::end(graph.m_nodes),
		              [index_of, &nodes_sorted, &node_index](auto const& node) {
			              node_index[&node] = index_of(nodes_sorted, &node);
		              });

		assert(node_index[&graph[4]] == 0);
		assert(node_index[&graph[0]] > node_index[&graph[1]]);
		assert(node_index[&graph[1]] > node_index[&graph[4]]);
		assert(node_index[&graph[2]] > node_index[&graph[4]]);
		assert(node_index[&graph[3]] > node_index[&graph[4]]);
	}
}


int main()
{
	Testcases::graphutilsVisitNodesDag();
	Testcases::graphutilsVisitNodesNotADag();
	Testcases::graphutilsProcessGraphNodeRecursive();
	Testcases::graphutilsProcessGraphNodeRecursiveSimpleDiamond();

	return 0;
}
