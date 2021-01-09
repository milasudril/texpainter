//@	{
//@	  "targets":[{"name":"compositor.hpp", "type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"compositor.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_MODEL_COMPOSITOR_HPP
#define TEXPAINTER_MODEL_COMPOSITOR_HPP

#include "./image_sink.hpp"

#include "filtergraph/graph.hpp"
#include "pixel_store/image.hpp"
#include "utils/iter_pair.hpp"
#include "utils/pair_iterator.hpp"
#include "sched/signaling_counter.hpp"
#include "sched/thread_pool.hpp"

#define JSON_USE_IMPLICIT_CONVERSIONS 0
#include <nlohmann/json.hpp>

#include <algorithm>

namespace Texpainter::Model
{
	class Compositor;

	Texpainter::FilterGraph::ValidationResult validate(Compositor const& g);

	class Compositor
	{
	public:
		static constexpr FilterGraph::NodeId OutputNodeId{0};
		using NodeItem        = FilterGraph::Graph::NodeItem;
		using NodeId          = FilterGraph::NodeId;
		using InputPortIndex  = FilterGraph::InputPortIndex;
		using OutputPortIndex = FilterGraph::OutputPortIndex;

		Compositor(): m_valid_state{ValidationState::NotValidated}
		{
			using FilterGraph::ImageProcessorWrapper;
			using FilterGraph::ImageSink;

			auto output   = std::make_unique<ImageProcessorWrapper<ImageSink>>(ImageSink{});
			r_output      = &output->processor();
			r_output_node = &m_graph.insert(std::move(output)).second.get();
		}

		Compositor(Compositor const& other) = delete;

		Compositor(Compositor&&) = default;
		Compositor& operator=(Compositor&&) = default;

		void process(Span2d<PixelStore::Pixel> canvas, double resolution) const;

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

		auto nodeCount() const { return m_graph.size(); }

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
		mutable Texpainter::Sched::ThreadPool m_workers;

		enum class ValidationState : size_t
		{
			NotValidated,
			ValidatedNotValid,
			ValidatedValid
		};
		mutable ValidationState m_valid_state;

		FilterGraph::ImageSink* r_output;
		FilterGraph::Node* r_output_node;

		FilterGraph::Graph m_graph;
	};

	Texpainter::FilterGraph::ValidationResult validate(Compositor const& g);

	std::map<FilterGraph::Node const*, FilterGraph::NodeId> mapNodesToNodeIds(Compositor const& g);

	void to_json(nlohmann::json& obj, Compositor const& src);
}

#endif