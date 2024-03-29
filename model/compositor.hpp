//@	{
//@	  "targets":[{"name":"compositor.hpp", "type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"compositor.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_MODEL_COMPOSITOR_HPP
#define TEXPAINTER_MODEL_COMPOSITOR_HPP

#include "./image_sink.hpp"
#include "./compositor_output.hpp"
#include "./topography_sink.hpp"

#include "filtergraph/graph.hpp"

#include "pixel_store/image.hpp"
#include "sched/signaling_counter.hpp"
#include "sched/thread_pool.hpp"


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

		Compositor(): m_current_size{0, 0}, m_valid_state{ValidationState::NotValidated}
		{
			using FilterGraph::ImageProcessorWrapper;

			{
				auto output   = std::make_unique<ImageProcessorWrapper<ImageSink>>(ImageSink{});
				r_output_node = &m_graph.insert(std::move(output)).second.get();
				r_default_output_node = r_output_node;
			}
		}

		Compositor(Compositor const& other) = delete;

		Compositor(Compositor&&) = default;
		Compositor& operator=(Compositor&&) = default;

		CompositorOutput process(Size2d canvas_size, uint32_t scale) const;

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
			clearValidationState();
			return *this;
		}

		bool checkedConnect(NodeId a, InputPortIndex sink, NodeId b, OutputPortIndex src)
		{
			if(!m_graph.checkedConnect(a, sink, b, src)) { return false; }
			clearValidationState();
			return true;
		}

		Compositor& disconnect(NodeId a, InputPortIndex sink)
		{
			m_graph.disconnect(a, sink);
			clearValidationState();
			return *this;
		}

		auto node(FilterGraph::NodeId id) const { return m_graph.node(id); }

		auto node(FilterGraph::NodeId id) { return m_graph.node(id); }

		Compositor& erase(FilterGraph::NodeId id)
		{
			if(id == r_output_node->nodeId()) { r_output_node = r_default_output_node; }
			m_graph.erase(id);
			clearValidationState();
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
					compile();
				}
			return m_valid_state == ValidationState::ValidatedValid && std::size(m_node_array) != 0;
		}

		void clearValidationState()
		{
			m_valid_state = ValidationState::NotValidated;
			m_node_array.clear();
		}

		auto nodeCount() const { return m_graph.size(); }

		auto nodeData() const { return FilterGraph::nodeData(m_graph); }

		FilterGraph::Node const& outputNode() const { return *r_output_node; }

		Compositor& outputNode(std::reference_wrapper<FilterGraph::Node const> node)
		{
			r_output_node = &node.get();
			clearValidationState();
			return *this;
		}

	private:
		struct Task
		{
			using Node = FilterGraph::Node;
			std::reference_wrapper<Node const> node;
			size_t task_id;
			std::array<size_t, Node::maxNumInputs()> dependent_tasks;
			size_t input_count;
		};
		void compile() const;
		mutable Size2d m_current_size;
		mutable std::vector<Task> m_node_array;
		mutable std::vector<std::atomic<bool>> m_node_status;
		mutable Texpainter::Sched::ThreadPool m_workers;

		enum class ValidationState : size_t
		{
			NotValidated,
			ValidatedNotValid,
			ValidatedValid
		};
		mutable ValidationState m_valid_state;

		FilterGraph::Node const* r_output_node;
		FilterGraph::Node const* r_default_output_node;

		FilterGraph::Graph m_graph;
	};

	Texpainter::FilterGraph::ValidationResult validate(Compositor const& g);

	std::map<FilterGraph::Node const*, FilterGraph::NodeId> mapNodesToNodeIds(Compositor const& g);

	inline auto nodeData(Compositor const& g) { return g.nodeData(); }

	inline auto processIfValid(Compositor const& compositor, Size2d canvas_size, uint32_t scale)
	{
		if(compositor.valid()) [[likely]]
			{
				return compositor.process(canvas_size, scale);
			}
		else
		{
			return CompositorOutput{canvas_size};
		}
	}
}

#endif