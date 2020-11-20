//@	{
//@	  "targets":[{"name":"compositor.hpp", "type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"compositor.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_MODEL_COMPOSITOR_HPP
#define TEXPAINTER_MODEL_COMPOSITOR_HPP

#include "filtergraph/graph.hpp"
#include "filtergraph/layer_input.hpp"
#include "filtergraph/image_sink.hpp"
#include "pixel_store/image.hpp"
#include "utils/iter_pair.hpp"
#include "utils/pair_iterator.hpp"
#include "sched/signaling_counter.hpp"

#include <algorithm>

namespace Texpainter::Model
{
	class Input
	{
	public:
		explicit Input(Span2d<PixelStore::Pixel const> pixels,
		               std::reference_wrapper<PixelStore::Palette<16> const> pal)
		    : r_pixels{pixels}
		    , m_palette{pal}
		{
		}

		auto pixels() const { return r_pixels; }

		auto palette() const { return m_palette; }

	private:
		Span2d<PixelStore::Pixel const> r_pixels;
		std::reference_wrapper<PixelStore::Palette<16> const> m_palette;
	};

	class Compositor;

	Texpainter::FilterGraph::ValidationResult validate(Compositor const& g);

	class Compositor
	{
	public:
		static constexpr FilterGraph::NodeId InputNodeId{0};
		static constexpr FilterGraph::NodeId OutputNodeId{1};
		using NodeItem        = FilterGraph::Graph::NodeItem;
		using NodeId          = FilterGraph::NodeId;
		using InputPortIndex  = FilterGraph::InputPortIndex;
		using OutputPortIndex = FilterGraph::OutputPortIndex;

		Compositor(): m_valid_state{ValidationState::NotValidated}
		{
			using FilterGraph::ImageProcessorWrapper;
			using FilterGraph::ImageSink;
			using FilterGraph::LayerInput;

			auto input    = std::make_unique<ImageProcessorWrapper<LayerInput>>(LayerInput{});
			auto output   = std::make_unique<ImageProcessorWrapper<ImageSink>>(ImageSink{});
			r_input       = &input->processor();
			r_output      = &output->processor();
			r_input_node  = &m_graph.insert(std::move(input)).second.get();
			r_output_node = &m_graph.insert(std::move(output)).second.get();

			connect(OutputNodeId,
			        FilterGraph::InputPortIndex{0},
			        InputNodeId,
			        FilterGraph::OutputPortIndex{0});
		}

		Compositor(Compositor const& other): m_graph{other.m_graph}
		{
			using FilterGraph::ImageProcessorWrapper;
			using FilterGraph::ImageSink;
			using FilterGraph::LayerInput;

			r_input_node  = m_graph.node(InputNodeId);
			r_output_node = m_graph.node(OutputNodeId);
			r_input = &dynamic_cast<ImageProcessorWrapper<LayerInput>*>(&r_input_node->processor())
			               ->processor();
			r_output = &dynamic_cast<ImageProcessorWrapper<ImageSink>*>(&r_output_node->processor())
			                ->processor();
		}

		Compositor(Compositor&&) = default;
		Compositor& operator=(Compositor&&) = default;

		PixelStore::Image process(Input const& input, bool force_update = true) const;

		NodeItem insert(std::unique_ptr<FilterGraph::AbstractImageProcessor> proc)
		{
			return m_graph.insert(std::move(proc));
		}

		template<FilterGraph::ImageProcessor ImgProc>
		NodeItem insert(ImgProc&& proc)
		{
			return m_graph.insert(std::forward<ImgProc>(proc));
		}

		Compositor& connect(NodeId a, InputPortIndex sink, NodeId b, OutputPortIndex src)
		{
			m_graph.connect(a, sink, b, src);
			m_valid_state = ValidationState::NotValidated;
			return *this;
		}

		Compositor& disconnect(NodeId a, InputPortIndex sink)
		{
			m_graph.disconnect(a, sink);
			m_valid_state = ValidationState::NotValidated;
			return *this;
		}

		auto node(FilterGraph::NodeId id) const { return m_graph.node(id); }

		Compositor& erase(FilterGraph::NodeId id)
		{
			m_graph.erase(id);
			m_valid_state = ValidationState::NotValidated;
			return *this;
		}

		auto nodesWithId() const { return m_graph.nodesWithId(); }

		auto nodesWithId() { return m_graph.nodesWithId(); }

		bool valid() const
		{
			using FilterGraph::ValidationResult;

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
			m_node_array.clear();
		}

	private:
		struct NodeState
		{
			using Node = FilterGraph::Node;
			std::reference_wrapper<Node const> node;
			std::unique_ptr<Sched::SignalingCounter<size_t>> counter;
			std::array<std::vector<Sched::SignalingCounter<size_t>*>, Node::MaxNumOutputs>
			    signal_counters{};
		};
		mutable std::vector<NodeState> m_node_array;

		enum class ValidationState : size_t
		{
			NotValidated,
			ValidatedNotValid,
			ValidatedValid
		};
		mutable ValidationState m_valid_state;

		FilterGraph::LayerInput* r_input;
		FilterGraph::ImageSink* r_output;
		FilterGraph::Node* r_input_node;
		FilterGraph::Node* r_output_node;

		FilterGraph::Graph m_graph;
	};

	Texpainter::FilterGraph::ValidationResult validate(Compositor const& g);
}

#endif