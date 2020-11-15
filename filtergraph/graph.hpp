//@	{
//@	 "targets":[{"name":"graph.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"graph.o", "rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_GRAPH_HPP
#define TEXPAINTER_FILTERGRAPH_GRAPH_HPP

#include "./node.hpp"
#include "./node_id.hpp"
#include "./layer_input.hpp"
#include "./image_sink.hpp"
#include "./image_processor_wrapper.hpp"

#include "pixel_store/image.hpp"
#include "utils/iter_pair.hpp"
#include "utils/pair_iterator.hpp"
#include "sched/signaling_counter.hpp"

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

	class Input
	{
	public:
		explicit Input(Span2d<RgbaValue const> pixels, std::reference_wrapper<Palette const> pal)
		    : r_pixels{pixels}
		    , m_palette{pal}
		{
		}

		auto pixels() const { return r_pixels; }

		auto palette() const { return m_palette; }

	private:
		Span2d<RgbaValue const> r_pixels;
		std::reference_wrapper<Palette const> m_palette;
	};

	class Graph
	{
		using NodeMap = std::map<NodeId, Node>;

	public:
		static constexpr NodeId InputNodeId{0};
		static constexpr NodeId OutputNodeId{1};

		using node_type = Node;

		using NodeItem = std::pair<NodeId, std::reference_wrapper<Node>>;

		Graph(): m_valid_state{ValidationState::NotValidated}
		{
			auto input  = std::make_unique<ImageProcessorWrapper<LayerInput>>(LayerInput{});
			auto output = std::make_unique<ImageProcessorWrapper<ImageSink>>(ImageSink{});
			r_input     = &input->processor();
			r_output    = &output->processor();
			r_input_node =
			    &m_nodes.insert(std::make_pair(InputNodeId, std::move(input))).first->second;
			r_output_node =
			    &m_nodes.insert(std::make_pair(OutputNodeId, std::move(output))).first->second;
			m_current_id = NodeId{2};

			connect(OutputNodeId, InputPortIndex{0}, InputNodeId, OutputPortIndex{0});
		}

		Graph(Graph const& other);

		Graph(Graph&& other) = default;

		Graph& operator=(Graph&& other) = default;

		Graph& operator=(Graph const& other)
		{
			Graph tmp{other};
			*this = std::move(tmp);
			return *this;
		}

		PixelStore::Image process(Input const& input, bool force_update = true) const;

		std::vector<Node const*> compile() const;

		auto node(NodeId id) const
		{
			auto ret = m_nodes.find(id);
			return ret != std::end(m_nodes) ? &ret->second : nullptr;
		}

		Graph& connect(NodeId a, InputPortIndex sink, NodeId b, OutputPortIndex src)
		{
			assert(node(a) != nullptr && node(b) != nullptr);
			m_nodes[a].connect(sink, m_nodes[b], src);
			m_valid_state = ValidationState::NotValidated;
			return *this;
		}

		Graph& disconnect(NodeId a, InputPortIndex sink)
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

		auto nodesWithId() const { return IterPair{std::begin(m_nodes), std::end(m_nodes)}; }

		auto nodesWithId() { return IterPair{std::begin(m_nodes), std::end(m_nodes)}; }

		auto nodes() const
		{
			using NodeIterator = PairIterator<1, NodeMap::const_iterator>;
			return IterPair{NodeIterator{std::begin(m_nodes)}, NodeIterator{std::end(m_nodes)}};
		}

		auto nodes()
		{
			using NodeIterator = PairIterator<1, NodeMap::iterator>;
			return IterPair{NodeIterator{std::begin(m_nodes)}, NodeIterator{std::end(m_nodes)}};
		}

		size_t size() const { return m_nodes.size(); }

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
			m_node_array.clear();
		}

	private:
		struct NodeState
		{
			std::reference_wrapper<Node const> node;
			std::unique_ptr<Sched::SignalingCounter<size_t>> counter;
			std::array<std::vector<Sched::SignalingCounter<size_t>*>, Node::MaxNumOutputs>
			    signal_counters{};
		};
		mutable std::vector<NodeState> m_node_array;

		LayerInput* r_input;
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
		NodeMap m_nodes;
		NodeId m_current_id;
	};

	template<class Visitor>
	void visitNodes(Visitor&& visitor, Graph const& g)
	{
		std::ranges::for_each(g.nodes(), std::forward<Visitor>(visitor));
	}
}

#endif