//@	{
//@	  "targets":[{"name":"compositor.hpp", "type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"compositor.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_MODEL_COMPOSITOR_HPP
#define TEXPAINTER_MODEL_COMPOSITOR_HPP

#include "./image_sink.hpp"
#include "./topography_sink.hpp"

#include "filtergraph/graph.hpp"

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

		Compositor(): m_valid_state{ValidationState::NotValidated}, r_topo_output_node{nullptr}
		{
			using FilterGraph::ImageProcessorWrapper;

			{
				auto output   = std::make_unique<ImageProcessorWrapper<ImageSink>>(ImageSink{});
				r_output_node = &m_graph.insert(std::move(output)).second.get();
			}
		}

		Compositor(Compositor const& other) = delete;

		Compositor(Compositor&&) = default;
		Compositor& operator=(Compositor&&) = default;

		FilterGraph::PortValue const& process(Size2d canvas_size, double resolution) const;

		auto addTopoOutput()
		{
			if(r_topo_output_node == nullptr)
			{
				using FilterGraph::ImageProcessorWrapper;
				auto output =
				    std::make_unique<ImageProcessorWrapper<TopographySink>>(TopographySink{});
				r_topo_output = &output->processor();
				auto ret      = m_graph.insert(std::move(output));

				m_topo_output_node_id = ret.first;
				r_topo_output_node    = &ret.second.get();
			}
			return std::pair{m_topo_output_node_id, std::ref(*r_topo_output_node)};
		}

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

		FilterGraph::Node* r_output_node;

		TopographySink* r_topo_output;
		FilterGraph::NodeId m_topo_output_node_id;
		FilterGraph::Node* r_topo_output_node;

		FilterGraph::Graph m_graph;
	};

	Texpainter::FilterGraph::ValidationResult validate(Compositor const& g);

	std::map<FilterGraph::Node const*, FilterGraph::NodeId> mapNodesToNodeIds(Compositor const& g);

	inline auto nodeData(Compositor const& g) { return g.nodeData(); }

	inline auto processIfValid(Compositor const& compositor, Size2d canvas_size, double scale)
	{
		if(compositor.valid()) [[likely]]
			{
				auto const& res = compositor.process(canvas_size, scale);
				return visit(
				    [canvas_size](auto const& result) -> PixelStore::Image {
					    auto value       = FilterGraph::makeInputPortValue(result);
					    using ResultType = decltype(value);
					    if constexpr(std::is_same_v<ResultType, PixelStore::Pixel const*>)
					    { return PixelStore::Image{Span2d{value, canvas_size}}; }
					    else
					    {
						    return PixelStore::Image{canvas_size};
					    }
				    },
				    res);
			}
		else
		{
			PixelStore::Image ret{canvas_size};
			std::ranges::fill(ret.pixels(), PixelStore::Pixel{0.0f, 0.0f, 0.0f, 0.0f});
			return ret;
		}
	}
}

#endif