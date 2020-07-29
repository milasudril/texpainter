//@	{
//@	 "targets":[{"name":"graph.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_GRAPH_HPP
#define TEXPAINTER_FILTERGRAPH_GRAPH_HPP

#include "./processor_node.hpp"
#include "./node_id.hpp"
#include "./image_source.hpp"
#include "./output_node.hpp"

#include "utils/iter_pair.hpp"

namespace Texpainter::FilterGraph
{
	class Graph
	{
	public:
		Graph(): m_output_node{OutputNode{}} {}

		template<class PixelType>
		Model::Image process(Span2d<PixelType const> source) const
		{
			m_input_node = ProcessorNode{ImageSource{source}};
			auto ret     = m_output_node();

			// NOTE: OutputNode always returns one Model::Image
			return *std::get_if<Model::Image>(&ret[0]);
		}

		Graph& connect(NodeId node, InputPort sink, OutputPort src)
		{
			m_nodes[node].connect(sink, m_input_node, src);
			return *this;
		}

		Graph& connect(InputPort sink, NodeId node, OutputPort src)
		{
			m_output_node.connect(sink, m_nodes[node], src);
			return *this;
		}

		Graph& connect(NodeId a, InputPort sink, NodeId b, OutputPort src)
		{
			m_nodes[a].connect(sink, m_nodes[b], src);
			return *this;
		}

		template<ImageProcessor T>
		std::pair<NodeId const, ProcessorNode>& insert(T&& obj)
		{
			auto i = m_nodes.insert(std::make_pair(m_current_id, std::forward<T>(obj)));
			++m_current_id;
			return *i.first;
		}

		auto nodes() const { return IterPair{std::begin(m_nodes), std::end(m_nodes)}; }

		auto currentId() const { return m_current_id; }

	private:
		mutable ProcessorNode m_input_node;
		ProcessorNode m_output_node;
		std::map<NodeId, ProcessorNode> m_nodes;
		NodeId m_current_id;
	};
}

#endif