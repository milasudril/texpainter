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
		PixelStore::Image process(Span2d<PixelType const> source) const
		{
			m_input_node = ProcessorNode{ImageSource{source}};
			auto ret     = m_output_node();
			if(ret.size() != 1) [[unlikely]]  // Should only have one output
				{
					PixelStore::Image ret{source.size()};
					std::ranges::fill(ret.pixels(), PixelStore::Pixel{0.0f, 0.0f, 0.0f, 0.0f});
				}

			// NOTE: OutputNode always returns PixelStore::Image
			return *std::get_if<PixelStore::Image>(&ret[0]);
		}

		Graph& connectInput(NodeId node, InputPort sink)
		{
			m_nodes[node].connect(sink, m_input_node, OutputPort{0});
			return *this;
		}

		Graph& connectOutput(NodeId node, OutputPort src)
		{
			m_output_node.connect(InputPort{0}, m_nodes[node], src);
			return *this;
		}

		Graph& connect(NodeId a, InputPort sink, NodeId b, OutputPort src)
		{
			m_nodes[a].connect(sink, m_nodes[b], src);
			return *this;
		}


		Graph& disconnectOutput()
		{
			m_output_node.disconnect(InputPort{0});
			return *this;
		}

		Graph& disconnect(NodeId a, InputPort sink)
		{
			m_nodes[a].disconnect(sink);
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

		auto get(NodeId id, std::string_view paramname) const
		{
			// Assume that id exists
			return m_nodes.find(id)->second.get(paramname);
		}

		void set(NodeId id, std::string_view paramname, ProcParamValue val)
		{
			// Assume that id exists
			m_nodes.find(id)->second.set(paramname, val);
		}

		auto currentId() const { return m_current_id; }

	private:
		mutable ProcessorNode m_input_node;
		ProcessorNode m_output_node;
		std::map<NodeId, ProcessorNode> m_nodes;
		NodeId m_current_id;
	};
}

#endif