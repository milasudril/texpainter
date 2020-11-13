//@	{
//@	  "targets":[{"name":"graphutils.test","type":"application", "autorun":1}]
//@	 }

#undef NDEBUG

#include "./graphutils.hpp"

#include <algorithm>
#include <cassert>

namespace
{
	struct Edge
	{
		Edge(int id): m_id{id} {}
		int m_id;
	};

	int reference(Edge e) { return e.m_id; }

	struct Node
	{
		int m_id;
		std::vector<Edge> m_edges;
	};

	template<class F>
	void visitEdges(F&& f, Node const& node)
	{
		std::for_each(std::begin(node.m_edges), std::end(node.m_edges), f);
	}

	int id(Node const& node) { return node.m_id; }

	struct Graph
	{
		using node_type = Node;
		std::vector<Node> m_nodes;
	};

	Node const& getNodeById(Graph const& g, int id) { return g.m_nodes[id]; }

	size_t size(Graph const& g) { return g.m_nodes.size(); }

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
		graph.m_nodes.push_back(Node{0, std::vector<Edge>{1, 2, 3}});
		graph.m_nodes.push_back(Node{1, std::vector<Edge>{4, 5, 6}});
		graph.m_nodes.push_back(Node{2, std::vector<Edge>{4, 7, 8}});
		graph.m_nodes.push_back(Node{3, std::vector<Edge>{8}});
		graph.m_nodes.push_back(Node{4, std::vector<Edge>{10}});
		graph.m_nodes.push_back(Node{5, std::vector<Edge>{10}});
		graph.m_nodes.push_back(Node{6, std::vector<Edge>{10}});
		graph.m_nodes.push_back(Node{7, std::vector<Edge>{10}});
		graph.m_nodes.push_back(Node{8, std::vector<Edge>{10}});
		graph.m_nodes.push_back(Node{9, std::vector<Edge>{4, 5, 6}});
		graph.m_nodes.push_back(Node{10, std::vector<Edge>{}});

		std::vector<Node const*> nodes_sorted;
		Texpainter::visitNodesInTopoOrder(
		    [&nodes_sorted](Node const& node) { nodes_sorted.push_back(&node); }, graph);

		auto index_of = [](auto const& nodes, int value) {
			auto i = std::find_if(std::begin(nodes), std::end(nodes), [value](auto node) {
				return node->m_id == value;
			});
			return i - std::begin(nodes);
		};

		auto s = size(graph);
		std::vector<ptrdiff_t> node_index(s);
		std::for_each(std::begin(graph.m_nodes),
		              std::end(graph.m_nodes),
		              [index_of, &nodes_sorted, &node_index](auto node) {
			              node_index[node.m_id] = index_of(nodes_sorted, node.m_id);
		              });

		assert(node_index[10] == 0);
		assert(node_index[10] < node_index[4]);
		assert(node_index[10] < node_index[5]);
		assert(node_index[10] < node_index[6]);
		assert(node_index[10] < node_index[7]);
		assert(node_index[10] < node_index[8]);
		assert(node_index[4] < node_index[1]);
		assert(node_index[4] < node_index[2]);
		assert(node_index[4] < node_index[9]);
		assert(node_index[5] < node_index[1]);
		assert(node_index[5] < node_index[9]);
		assert(node_index[6] < node_index[1]);
		assert(node_index[6] < node_index[9]);
		assert(node_index[7] < node_index[2]);
		assert(node_index[8] < node_index[2]);
		assert(node_index[8] < node_index[3]);
		assert(node_index[1] < node_index[0]);
		assert(node_index[2] < node_index[0]);
		assert(node_index[3] < node_index[0]);
	}

	void graphutilsVisitNodesNoDag()
	{
		Graph graph;
		graph.m_nodes.push_back(Node{0, std::vector<Edge>{1, 2, 3}});
		graph.m_nodes.push_back(Node{1, std::vector<Edge>{4, 5, 6}});
		graph.m_nodes.push_back(Node{2, std::vector<Edge>{4, 7, 8}});
		graph.m_nodes.push_back(Node{3, std::vector<Edge>{8}});
		graph.m_nodes.push_back(Node{4, std::vector<Edge>{10}});
		graph.m_nodes.push_back(Node{5, std::vector<Edge>{10}});
		graph.m_nodes.push_back(Node{6, std::vector<Edge>{10}});
		graph.m_nodes.push_back(Node{7, std::vector<Edge>{10}});
		graph.m_nodes.push_back(Node{8, std::vector<Edge>{10}});
		graph.m_nodes.push_back(Node{9, std::vector<Edge>{4, 5, 6}});
		graph.m_nodes.push_back(Node{10, std::vector<Edge>{9}});

		try
		{
			Texpainter::visitNodesInTopoOrder([](Node const&) { abort(); }, graph);
			abort();
		}
		catch(...)
		{
		}
	}

	void graphutilsProcessGraphNodeRecursive()
	{
		Graph graph;
		graph.m_nodes.push_back(Node{0, std::vector<Edge>{1, 2, 3}});
		graph.m_nodes.push_back(Node{1, std::vector<Edge>{4, 5, 6}});
		graph.m_nodes.push_back(Node{2, std::vector<Edge>{4, 7, 8}});
		graph.m_nodes.push_back(Node{3, std::vector<Edge>{8}});
		graph.m_nodes.push_back(Node{4, std::vector<Edge>{10}});
		graph.m_nodes.push_back(Node{5, std::vector<Edge>{10}});
		graph.m_nodes.push_back(Node{6, std::vector<Edge>{10}});
		graph.m_nodes.push_back(Node{7, std::vector<Edge>{10}});
		graph.m_nodes.push_back(Node{8, std::vector<Edge>{10}});
		graph.m_nodes.push_back(Node{9, std::vector<Edge>{4, 5, 6}});
		graph.m_nodes.push_back(Node{10, std::vector<Edge>{}});

		std::vector<Node const*> nodes_sorted;
		Texpainter::processGraphNodeRecursive(
		    [&nodes_sorted](Node const& node) { nodes_sorted.push_back(&node); },
		    graph,
		    graph.m_nodes[9]);

		auto index_of = [](auto const& nodes, int value) {
			auto i = std::find_if(std::begin(nodes), std::end(nodes), [value](auto node) {
				return node->m_id == value;
			});
			return i - std::begin(nodes);
		};

		auto s = size(graph);
		std::vector<ptrdiff_t> node_index(s);
		std::for_each(std::begin(graph.m_nodes),
		              std::end(graph.m_nodes),
		              [index_of, &nodes_sorted, &node_index](auto node) {
			              node_index[node.m_id] = index_of(nodes_sorted, node.m_id);
		              });

		assert(node_index[10] == 0);
		assert(node_index[4] < node_index[9]);
		assert(node_index[5] < node_index[9]);
		assert(node_index[6] < node_index[9]);
	}

	void graphutilsProcessGraphNodeRecursiveSimpleDiamond()
	{
		Graph graph;
		graph.m_nodes.push_back(Node{0, std::vector<Edge>{1, 2, 3}});
		graph.m_nodes.push_back(Node{1, std::vector<Edge>{4}});
		graph.m_nodes.push_back(Node{2, std::vector<Edge>{4}});
		graph.m_nodes.push_back(Node{3, std::vector<Edge>{4}});
		graph.m_nodes.push_back(Node{4, std::vector<Edge>{}});


		std::vector<Node const*> nodes_sorted;
		Texpainter::processGraphNodeRecursive(
		    [&nodes_sorted](Node const& node) { nodes_sorted.push_back(&node); },
		    graph,
		    graph.m_nodes[0]);

		auto index_of = [](auto const& nodes, int value) {
			auto i = std::find_if(std::begin(nodes), std::end(nodes), [value](auto node) {
				return node->m_id == value;
			});
			return i - std::begin(nodes);
		};

		auto s = size(graph);
		std::vector<ptrdiff_t> node_index(s);
		std::for_each(std::begin(graph.m_nodes),
		              std::end(graph.m_nodes),
		              [index_of, &nodes_sorted, &node_index](auto node) {
			              node_index[node.m_id] = index_of(nodes_sorted, node.m_id);
		              });

		assert(node_index[4] == 0);
		assert(node_index[0] > node_index[1]);
		assert(node_index[1] > node_index[4]);
		assert(node_index[2] > node_index[4]);
		assert(node_index[3] > node_index[4]);
	}
}


int main()
{
	Testcases::graphutilsVisitNodesDag();
	Testcases::graphutilsVisitNodesNoDag();
	Testcases::graphutilsProcessGraphNodeRecursive();
	Testcases::graphutilsProcessGraphNodeRecursiveSimpleDiamond();

	return 0;
}