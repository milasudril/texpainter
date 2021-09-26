//@	{
//@	  "targets":[{"name":"compositor_new.hpp", "type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"compositor_new.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_MODEL_COMPOSITORNEW_HPP
#define TEXPAINTER_MODEL_COMPOSITORNEW_HPP

#include "./image_sink.hpp"

#include "filtergraph/graph_new.hpp"
#include "filtergraph/node_output.hpp"

#include "pixel_store/image.hpp"
#include "sched/signaling_counter.hpp"
#include "sched/thread_pool.hpp"


#define JSON_USE_IMPLICIT_CONVERSIONS 0
#include <nlohmann/json.hpp>

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

		bool checkedConnect(NodeId a, InputPortIndex sink, NodeId b, OutputPortIndex src)
		{
			if(!m_graph.checkedConnect(a, sink, b, src)) { return false; }
			m_valid_state = ValidationState::NotValidated;
			return true;
		}

		Compositor& disconnect(NodeId a, InputPortIndex sink)
		{
			m_graph.disconnect(a, sink);
			m_valid_state = ValidationState::NotValidated;
			return *this;
		}

		auto node(FilterGraph::NodeId id) const { return m_graph.node(id); }

		auto node(FilterGraph::NodeId id) { return m_graph.node(id); }

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

		auto nodeData() const { return FilterGraph::nodeData(m_graph); }

	private:
		mutable std::vector<std::reference_wrapper<FilterGraph::Node const>> m_node_array;
		mutable Texpainter::Sched::ThreadPool m_workers;

		struct Task
		{
			using Node = FilterGraph::Node;

			enum class Status : int
			{
				WaitingForInputs,
				Ready
			};

			std::reference_wrapper<Node const> node;
			std::array<size_t, Node::maxNumInputs()> task_inputs;
			Status status;
		};

		enum class ValidationState : size_t
		{
			NotValidated,
			ValidatedNotValid,
			ValidatedValid
		};
		mutable ValidationState m_valid_state;

		ImageSink* r_output;
		FilterGraph::Node* r_output_node;

		mutable std::map<FilterGraph::NodeId, FilterGraph::NodeOutput> m_node_output;
		FilterGraph::Graph m_graph;
	};

	Texpainter::FilterGraph::ValidationResult validate(Compositor const& g);

	std::map<FilterGraph::Node const*, FilterGraph::NodeId> mapNodesToNodeIds(Compositor const& g);

	inline auto nodeData(Compositor const& g) { return g.nodeData(); }
}

#endif