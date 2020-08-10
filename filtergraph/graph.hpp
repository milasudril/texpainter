//@	{
//@	 "targets":[{"name":"graph.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"graph.o", "rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_GRAPH_HPP
#define TEXPAINTER_FILTERGRAPH_GRAPH_HPP

#include "./node.hpp"
#include "./node_id.hpp"
#include "./image_source.hpp"
#include "./output_node.hpp"

#include "utils/iter_pair.hpp"

#include "imgproc/rgba_split/rgba_split.hpp"
#include "imgproc/rgba_combine/rgba_combine.hpp"

#include <algorithm>

namespace Texpainter::FilterGraph
{
	class Graph
	{
	public:
		static constexpr NodeId InputNodeId{0};
		static constexpr NodeId OutputNodeId{1};

		Graph()
		{
			r_input  = &m_nodes[InputNodeId];  // Create an empty node as input node
			r_output = &m_nodes.insert(std::make_pair(OutputNodeId, OutputNode{})).first->second;
			m_current_id = NodeId{2};

			insert(RgbaSplit::ImageProcessor{});
			insert(RgbaCombine::ImageProcessor{});

			connect(NodeId{2}, InputPort{0}, NodeId{0}, OutputPort{0});

			connect(NodeId{3}, InputPort{0}, NodeId{2}, OutputPort{1});
			connect(NodeId{3}, InputPort{1}, NodeId{2}, OutputPort{2});
			connect(NodeId{3}, InputPort{2}, NodeId{2}, OutputPort{0});
			connect(NodeId{3}, InputPort{3}, NodeId{2}, OutputPort{3});

			connect(NodeId{1}, InputPort{0}, NodeId{3}, OutputPort{0});
		}

		Graph(Graph const& other);

		template<class PixelType>
		PixelStore::Image process(Span2d<PixelType const> source) const
		{
			r_input->replaceWith(ImageSource{source});
			auto ret = (*r_output)();
			if(ret.size() != 1) [[unlikely]]  // Should only have one output
				{
					PixelStore::Image ret{source.size()};
					std::ranges::fill(ret.pixels(), PixelStore::Pixel{0.0f, 0.0f, 0.0f, 0.0f});
				}

			// NOTE: OutputNode always returns PixelStore::Image
			return *std::get_if<PixelStore::Image>(&ret[0]);
		}


		Graph& connect(NodeId a, InputPort sink, NodeId b, OutputPort src)
		{
			m_nodes[a].connect(sink, m_nodes[b], src);
			return *this;
		}

		Graph& disconnect(NodeId a, InputPort sink)
		{
			m_nodes[a].disconnect(sink);
			return *this;
		}


		template<ImageProcessor T>
		std::pair<NodeId, std::reference_wrapper<Node const>> insert(T&& obj)
		{
			auto i = m_nodes.insert(std::make_pair(m_current_id, std::forward<T>(obj)));
			++m_current_id;
			return std::make_pair(i.first->first, std::ref(i.first->second));
		}

		Graph& erase(NodeId id)
		{
			m_nodes.erase(id);
			return *this;
		}

		auto nodes() const { return IterPair{std::begin(m_nodes), std::end(m_nodes)}; }

		auto get(NodeId id, std::string_view paramname) const
		{
			// Assume that id exists
			return m_nodes.find(id)->second.get(paramname);
		}

		void set(NodeId id, std::string_view paramname, ParamValue val)
		{
			// Assume that id exists
			m_nodes.find(id)->second.set(paramname, val);
		}

		auto currentId() const { return m_current_id; }

		auto node(NodeId id) const
		{
			auto ret = m_nodes.find(id);
			return ret != std::end(m_nodes) ? &ret->second : nullptr;
		}

	private:
		Node* r_input;
		Node* r_output;
		std::map<NodeId, Node> m_nodes;
		NodeId m_current_id;
	};
}

#endif