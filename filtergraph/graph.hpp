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

		PixelStore::Image process(Span2d<RgbaValue const> input, bool force_update = true) const
		{
			assert(valid());
			r_input->source(input);
			if(force_update) { r_input_node->forceUpdate(); }

			PixelStore::Image ret{input.size()};
			r_output->sink(ret.pixels());
			// NOTE: Since OutputNode does not use the internal image cache (it has no outputs)
			//       and it may happen that it is not connected to the input node, we must always
			//       recompute the output node. Otherwise, the contents of ret will be undefined,
			//       in case we already have computed the output result.
			r_output_node->forceUpdate();
			(*r_output_node)(input.size());
			return ret;
		}

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

		void clearValidationState() { m_valid_state = ValidationState::NotValidated; }

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