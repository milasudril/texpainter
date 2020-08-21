//@	{
//@	 "targets":[{"name":"graph.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"graph.o", "rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_GRAPH_HPP
#define TEXPAINTER_FILTERGRAPH_GRAPH_HPP

#include "./node.hpp"
#include "./node_id.hpp"
#include "./image_source.hpp"
#include "./image_sink.hpp"
#include "./image_processor_wrapper.hpp"

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
			auto input = std::make_unique<ImageProcessorWrapper<ImageSource<RgbaValue>>>(
			    ImageSource<RgbaValue>{});
			auto output = std::make_unique<ImageProcessorWrapper<ImageSink>>(ImageSink{});
			r_input     = &input->processor();
			r_output    = &output->processor();
			r_input_node =
			    &m_nodes.insert(std::make_pair(InputNodeId, std::move(input))).first->second;
			r_output_node =
			    &m_nodes.insert(std::make_pair(OutputNodeId, std::move(output))).first->second;
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

		PixelStore::Image process(Span2d<RgbaValue const> input) const
		{
			r_input->source(input);
			PixelStore::Image ret{input.size()};
			r_output->pixels(ret.pixels());
			r_input_node->forceUpdate();
			(*r_output_node)(input.size());
			return ret;
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

		std::pair<NodeId, std::reference_wrapper<Node const>> insert(
		    std::unique_ptr<AbstractImageProcessor> proc)
		{
			auto i = m_nodes.insert(std::make_pair(m_current_id, std::move(proc)));
			++m_current_id;
			return std::make_pair(i.first->first, std::ref(i.first->second));
		}

		template<ImageProcessor ImgProc>
		std::pair<NodeId, std::reference_wrapper<Node const>> insert(ImgProc&& proc)
		{
			return insert(
			    std::make_unique<ImageProcessorWrapper<ImgProc>>(std::forward<ImgProc>(proc)));
		}
#if 0

		Graph& erase(NodeId id)
		{
			m_nodes.erase(id);
			return *this;
		}
#endif
		auto nodes() const { return IterPair{std::begin(m_nodes), std::end(m_nodes)}; }
#if 0
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
#endif

	private:
		ImageSource<RgbaValue>* r_input;
		ImageSink* r_output;
		Node* r_input_node;
		Node* r_output_node;
		std::map<NodeId, Node> m_nodes;
		NodeId m_current_id;
	};
}

#endif