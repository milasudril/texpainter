//@	{
//@	 "targets":[{"name":"graph.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_GRAPH_HPP
#define TEXPAINTER_FILTERGRAPH_GRAPH_HPP

#include "./processor_node.hpp"

#include <string>

namespace Texpainter::FilterGraph
{
	class Graph
	{
	public:
		using NodeId = std::string

		Model::Image process(Span2d<Model::Pixel const> source) const
		{
			m_input_node = ProcessorNode{ImageSource{source}};
			return m_output_node();
		}

		Graph& connect(NodeId node, InputPort sink, OutputPort src)
		{
			m_nodes[node].connect(sink, m_input_node, src);
			return *this
		}

		Graph& connect(InputPort sink, NodeId node, OutputPort src)
		{
			m_output_node.connect(sink, m_nodes[node], src);
			return *this
		}

		Graph& connect(NodeId a, InputPort sink, NodeId b, OutputPort src)
		{
			m_nodes[a].connect(sink, m_nodes[b], src);
			return *this
		}

	private:
		mutable ProcessorNode m_input_node;
		ProcessorNode m_output_node;
		std::map<NodeId, ProcessorNode> m_nodes
	};
}

#endif