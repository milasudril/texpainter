//@	{
//@	 "targets":[{"name":"graph.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"graph.o", "rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_GRAPH_HPP
#define TEXPAINTER_FILTERGRAPH_GRAPH_HPP

#include "./node.hpp"
#include "./node_id.hpp"
#include "./image_processor_wrapper.hpp"

#include "pixel_store/image.hpp"
#include "utils/iter_pair.hpp"
#include "utils/pair_iterator.hpp"
#include "sched/signaling_counter.hpp"

#include <algorithm>

namespace Texpainter::FilterGraph
{
	enum class ValidationResult : int
	{
		NoError,
		InputsNotConnected,
		CyclicConnections,
		GraphTooDeep
	};

	class Graph
	{
		using NodeMap = std::map<NodeId, Node>;

	public:
		using node_type = Node;

		using NodeItem = std::pair<NodeId, std::reference_wrapper<Node>>;

		Graph() = default;

		Graph(Graph const& other);

		Graph(Graph&& other) = default;

		Graph& operator=(Graph&& other) = default;

		Graph& operator=(Graph const& other)
		{
			Graph tmp{other};
			*this = std::move(tmp);
			return *this;
		}

		auto node(NodeId id) const
		{
			auto ret = m_nodes.find(id);
			return ret != std::end(m_nodes) ? &ret->second : nullptr;
		}

		auto node(NodeId id)
		{
			auto ret = m_nodes.find(id);
			return ret != std::end(m_nodes) ? &ret->second : nullptr;
		}

		Graph& connect(NodeId a, InputPortIndex sink, NodeId b, OutputPortIndex src)
		{
			assert(node(a) != nullptr && node(b) != nullptr);
			m_nodes[a].connect(sink, m_nodes[b], src);
			return *this;
		}

		Graph& disconnect(NodeId a, InputPortIndex sink)
		{
			assert(node(a) != nullptr);
			m_nodes[a].disconnect(sink);
			return *this;
		}

		NodeItem insert(std::unique_ptr<AbstractImageProcessor> proc)
		{
			auto i = m_nodes.try_emplace(m_current_id, std::move(proc), m_current_id);
			++m_current_id;
			return std::make_pair(i.first->first, std::ref(i.first->second));
		}

		template<ImageProcessor ImgProc>
		NodeItem insert(ImgProc&& proc)
		{
			return insert(
			    std::make_unique<ImageProcessorWrapper<ImgProc>>(std::forward<ImgProc>(proc)));
		}

		Graph& erase(NodeId id)
		{
			m_nodes.erase(id);
			return *this;
		}

		auto nodesWithId() const { return IterPair{std::begin(m_nodes), std::end(m_nodes)}; }

		auto nodesWithId() { return IterPair{std::begin(m_nodes), std::end(m_nodes)}; }

		auto nodes() const
		{
			using NodeIterator = PairIterator<1, NodeMap::const_iterator>;
			return IterPair{NodeIterator{std::begin(m_nodes)}, NodeIterator{std::end(m_nodes)}};
		}

		auto nodes()
		{
			using NodeIterator = PairIterator<1, NodeMap::iterator>;
			return IterPair{NodeIterator{std::begin(m_nodes)}, NodeIterator{std::end(m_nodes)}};
		}

		size_t size() const { return m_nodes.size(); }

		auto get(NodeId id, ParamName paramname) const
		{
			assert(node(id) != nullptr);
			return m_nodes.find(id)->second.get(paramname);
		}

		void set(NodeId id, ParamName paramname, ParamValue val)
		{
			assert(node(id) != nullptr);
			m_nodes.find(id)->second.set(paramname, val);
		}

		auto currentId() const { return m_current_id; }

	private:
		NodeMap m_nodes;
		NodeId m_current_id;
	};

	template<class Visitor>
	void visitNodes(Visitor&& visitor, Graph const& g)
	{
		std::ranges::for_each(g.nodes(), std::forward<Visitor>(visitor));
	}

	inline std::map<NodeId, NodeData> nodeData(Graph const& g)
	{
		std::map<NodeId, NodeData> ret;
		std::ranges::transform(
		    g.nodesWithId(), std::inserter(ret, std::end(ret)), [](auto const& item) {
			    return std::pair{item.first, nodeData(item.second)};
		    });
		return ret;
	}
}

#endif