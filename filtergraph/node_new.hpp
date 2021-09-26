//@	{
//@	 "targets":[{"name":"node_new.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_NODENEW_HPP
#define TEXPAINTER_FILTERGRAPH_NODENEW_HPP

#include "./abstract_image_processor.hpp"
#include "./node_id.hpp"

#define JSON_USE_IMPLICIT_CONVERSIONS 0
#include <nlohmann/json.hpp>

#include <map>
#include <set>
#include <cassert>

namespace Texpainter::FilterGraph
{
	class Node;

	inline bool isConnected(Node const& node);

	class Node
	{
	public:
		class Source
		{
		public:
			Source(): r_processor{nullptr}, m_index{OutputPortIndex{0}} {}

			explicit Source(Node const* node, OutputPortIndex index)
			    : r_processor{node}
			    , m_index{index}
			{
			}

			Node const& processor() const { return *r_processor; }

			OutputPortIndex port() const { return m_index; }

			bool valid() const { return r_processor != nullptr; }

		private:
			Node const* r_processor;
			OutputPortIndex m_index;
		};

		using result_type = AbstractImageProcessor::result_type;

		static constexpr size_t MaxNumOutputs = AbstractImageProcessor::MaxNumOutputs;

		explicit Node(std::unique_ptr<AbstractImageProcessor> proc, NodeId id)
		    : m_proc{std::move(proc)}
		    , m_id{id}
		{
		}

		Node(Node&&) = delete;

		Node(): m_proc{nullptr}, m_id{InvalidNodeId} {}

		auto clonedProcessor() const { return m_proc->clone(); }

		AbstractImageProcessor const& processor() const { return *m_proc.get(); }

		AbstractImageProcessor& processor() { return *m_proc.get(); }

		bool hasProcessor() const { return m_proc != nullptr; }

		auto inputPorts() const { return m_proc->inputPorts(); }

		auto outputPorts() const { return m_proc->outputPorts(); }

		Node& connect(InputPortIndex input,
		              std::reference_wrapper<Node const> other,
		              OutputPortIndex output)
		{
			assert(input.value() < inputPorts().size());
			assert(output.value() < other.get().outputPorts().size());
			assert(inputPorts()[input.value()].type
			       == other.get().outputPorts()[output.value()].type);
			other.get().r_consumers[this].insert(input);

			if(m_inputs[input.value()].valid())
			{ m_inputs[input.value()].processor().r_consumers.find(this)->second.erase(input); }

			m_inputs[input.value()] = Source{&other.get(), output};
			return *this;
		}

		bool checkedConnect(InputPortIndex input,
		                    std::reference_wrapper<Node const> other,
		                    OutputPortIndex output)
		{
			// TODO: Return status code
			if(input.value() >= inputPorts().size()) { return false; }

			if(output.value() >= other.get().outputPorts().size()) { return false; }

			if(inputPorts()[input.value()].type != other.get().outputPorts()[output.value()].type)
			{ return false; }

			other.get().r_consumers[this].insert(input);

			if(m_inputs[input.value()].valid())
			{ m_inputs[input.value()].processor().r_consumers.find(this)->second.erase(input); }

			m_inputs[input.value()] = Source{&other.get(), output};
			return true;
		}

		Node& disconnect(InputPortIndex input)
		{
			assert(input.value() < NodeArgument::MaxNumInputs);
			m_inputs[input.value()].processor().r_consumers.find(this)->second.erase(input);
			m_inputs[input.value()] = Source{};
			return *this;
		}

		static constexpr auto maxNumInputs() { return NodeArgument::MaxNumInputs; }

		bool isConnected(InputPortIndex input) const
		{
			assert(input.value() < NodeArgument::MaxNumInputs);
			return m_inputs[input.value()].valid();
		}

		std::span<Source const> inputs() const
		{
			return {std::begin(m_inputs), std::begin(m_inputs) + inputPorts().size()};
		}

		auto nodeId() const { return m_id; }

		~Node()
		{
			std::ranges::for_each(r_consumers, [](auto const& item) {
				std::ranges::for_each(item.second, [proc = item.first](auto port) {
					// NOTE: Do not call disconnect here. This would screw up iterators.
					//       Also, r_consumers will be destroyed anywas.
					if(proc != nullptr) { proc->m_inputs[port.value()] = Source{}; }
				});
			});

			// NOTE: Remember to also disconnect all inputs. Otherwise, there will
			//       be dead pointers left in the producer.
			for(size_t k = 0; k < m_inputs.size(); ++k)
			{
				auto const port = InputPortIndex{static_cast<uint32_t>(k)};
				if(isConnected(port)) { disconnect(port); }
			}
		}

	private:
		std::array<Source, NodeArgument::MaxNumInputs> m_inputs;
		std::unique_ptr<AbstractImageProcessor> m_proc;
		NodeId m_id;

		struct InputPortIndexCompare
		{
			constexpr bool operator()(InputPortIndex a, InputPortIndex b) const
			{
				return a.value() < b.value();
			}
		};


		// NOTE: Using mutable here is not a very good solution, but allows treating producer nodes
		//       as const from the consumer side.
		//
		// TODO: Using std::set here is slightly overkill when there are only 4 ports
		mutable std::map<Node*, std::set<InputPortIndex, InputPortIndexCompare>> r_consumers;
	};

	inline bool isConnected(Node const& node)
	{
		return node.hasProcessor()
		       && std::ranges::none_of(node.inputs(), [](auto node) { return !node.valid(); });
	}

	template<class Visitor>
	void visitEdges(Visitor&& visitor, Node const& node)
	{
		std::ranges::for_each(node.inputs(), std::forward<Visitor>(visitor));
	}

	inline auto reference(Node::Source const& src)
	{
		return src.valid() ? &src.processor() : nullptr;
	}

	inline std::map<std::string, double> params(AbstractImageProcessor const& proc)
	{
		std::map<std::string, double> ret;
		auto param_names = proc.paramNames();
		std::ranges::transform(
		    param_names, std::inserter(ret, std::begin(ret)), [&proc](auto const& name) {
			    return std::pair{std::string{name.c_str()}, proc.get(name).value()};
		    });
		return ret;
	}

	struct NodeSourceData
	{
		NodeId node{InvalidNodeId};
		OutputPortIndex output_port{OutputPortNotConnected};
	};

	inline void to_json(nlohmann::json& obj, NodeSourceData const& node_input)
	{
		obj["node"]        = node_input.node;
		obj["output_port"] = node_input.output_port;
	}

	inline void from_json(nlohmann::json const& obj, NodeSourceData& node_input)
	{
		node_input.node        = obj.at("node").get<NodeId>();
		node_input.output_port = obj.at("output_port").get<OutputPortIndex>();
	}

	struct NodeData
	{
		ImageProcessorId imgproc;
		ImgProcReleaseState proc_relstate;
		std::array<NodeSourceData, NodeArgument::MaxNumInputs> inputs;
		std::map<std::string, double> params;
	};

	inline NodeData nodeData(Node const& node)
	{
		auto& proc = node.processor();
		NodeData ret{proc.id(), proc.releaseState(), {}, params(proc)};
		std::ranges::transform(node.inputs(), std::begin(ret.inputs), [](auto const& item) {
			if(item.valid()) { return NodeSourceData{item.processor().nodeId(), item.port()}; }
			return NodeSourceData{};
		});

		return ret;
	}

	inline void to_json(nlohmann::json& obj, NodeData const& node)
	{
		obj["imgproc"]       = node.imgproc;
		obj["inputs"]        = node.inputs;
		obj["params"]        = node.params;
		obj["proc_relstate"] = node.proc_relstate;
	}

	inline void from_json(nlohmann::json const& obj, NodeData& node)
	{
		node.imgproc       = obj.at("imgproc").get<ImageProcessorId>();
		node.proc_relstate = ImgProcReleaseState::Stable;
		if(auto i = obj.find("proc_relstate"); i != std::end(obj)) { node.proc_relstate = *i; }

		auto inputs = obj.at("inputs").get<std::vector<NodeSourceData>>();
		if(std::size(inputs) > NodeArgument::MaxNumInputs)
		{ throw std::string{"Too many inputs for node"}; }

		std::ranges::copy(inputs, std::begin(node.inputs));
		node.params = obj.at("params").get<std::map<std::string, double>>();
	}
}

#endif