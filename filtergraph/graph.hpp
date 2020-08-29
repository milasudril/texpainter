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
	enum class ValidationResult : int
	{
		NoError,
		InputsNotConnected,
		CyclicConnections,
		GraphTooDeep
	};

	class Graph;

	ValidationResult validate(Graph const& g);

	class Graph
	{
	public:
		static constexpr NodeId InputNodeId{0};
		static constexpr NodeId OutputNodeId{1};

		using NodeItem = std::pair<NodeId, std::reference_wrapper<Node>>;

		Graph(): m_valid_state{ValidationState::NotValidated}
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

		PixelStore::Image process(Span2d<RgbaValue const> input, bool force_update = true) const
		{
			assert(valid());
			r_input->source(input);
			PixelStore::Image ret{input.size()};
			r_output->sink(ret.pixels());
			if(force_update) { r_input_node->forceUpdate(); }
			(*r_output_node)(input.size());
			return ret;
		}

		auto node(NodeId id) const
		{
			auto ret = m_nodes.find(id);
			return ret != std::end(m_nodes) ? &ret->second : nullptr;
		}

		Graph& connect(NodeId a, InputPort sink, NodeId b, OutputPort src)
		{
			assert(node(a) != nullptr && node(b) != nullptr);
			m_nodes[a].connect(sink, m_nodes[b], src);
			m_valid_state = ValidationState::NotValidated;
			return *this;
		}

		Graph& disconnect(NodeId a, InputPort sink)
		{
			assert(node(a) != nullptr);
			m_nodes[a].disconnect(sink);
			m_valid_state = ValidationState::NotValidated;
			return *this;
		}

		NodeItem insert(std::unique_ptr<AbstractImageProcessor> proc)
		{
			auto i = m_nodes.insert(std::make_pair(m_current_id, std::move(proc)));
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
			assert(id != InputNodeId && id != OutputNodeId);
			m_nodes.erase(id);
			m_valid_state = ValidationState::NotValidated;
			return *this;
		}

		auto nodes() const { return IterPair{std::begin(m_nodes), std::end(m_nodes)}; }

		auto nodes() { return IterPair{std::begin(m_nodes), std::end(m_nodes)}; }

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

		bool valid() const
		{
			if(m_valid_state == ValidationState::NotValidated) [[unlikely]]
				{
					m_valid_state = validate(*this) == ValidationResult::NoError
					                    ? ValidationState::ValidatedValid
					                    : ValidationState::ValidatedNotValid;
				}
			return m_valid_state == ValidationState::ValidatedValid;
		}

		void clearValidationState()
		{
			m_valid_state = ValidationState::NotValidated;
		}

	private:
		ImageSource<RgbaValue>* r_input;
		ImageSink* r_output;
		Node* r_input_node;
		Node* r_output_node;
		enum class ValidationState : size_t
		{
			NotValidated,
			ValidatedNotValid,
			ValidatedValid
		};
		mutable ValidationState m_valid_state;
		std::map<NodeId, Node> m_nodes;
		NodeId m_current_id;
	};
}

#endif