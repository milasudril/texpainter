//@	{
//@	 "targets":[{"name":"node.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"./node.o", "rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_NODE_HPP
#define TEXPAINTER_FILTERGRAPH_NODE_HPP

#include "./abstract_image_processor.hpp"
#include "./node_id.hpp"

#include "utils/default_rng.hpp"

#include <nlohmann/json.hpp>

#include <map>
#include <set>
#include <atomic>
#include <cassert>
#include <optional>

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

			auto const& result() const
			{
				assert(valid());
				auto const& ret = r_processor->result();
				return ret[m_index.value()];
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
		    : m_last_modified{now()}
		    , m_last_rendered{0}
		    , m_result_cache_size{Size2d{0, 0}}
		    , m_proc{std::move(proc)}
		    , m_id{id}
		    , m_rng_seed{DefaultRng::genSeed()}
		{
		}

		Node(Node&&) = delete;

		Node()
		    : m_last_modified{now()}
		    , m_last_rendered{0}
		    , m_result_cache_size{Size2d{0, 0}}
		    , m_proc{nullptr}
		    , m_id{InvalidNodeId}
		{
		}

		auto clonedProcessor() const { return m_proc->clone(); }

		AbstractImageProcessor const& processor() const { return *m_proc.get(); }

		AbstractImageProcessor& processor() { return *m_proc.get(); }

		bool hasProcessor() const { return m_proc != nullptr; }

		auto inputPorts() const { return m_proc->inputPorts(); }

		auto outputPorts() const { return m_proc->outputPorts(); }

		result_type const& operator()(Size2d size, float resolution) const
		{
			assert(FilterGraph::isConnected(*this));

			std::array<InputPortValue, NodeArgument::MaxNumInputs> args{};
			auto const n_ports   = inputPorts().size();
			auto const input_end = std::begin(m_inputs) + n_ports;
			std::transform(std::begin(m_inputs), input_end, std::begin(args), [](auto const& val) {
				return makeInputPortValue(val.result());
			});
			m_result_cache      = (*m_proc)(NodeArgument{size, resolution, m_rng_seed, args});
			m_result_cache_size = size;
			m_last_rendered     = now();
			return m_result_cache;
		}

		result_type const& result() const
		{
			assert(m_last_modified < m_last_rendered);
			return m_result_cache;
		}

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
			touch();
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
			touch();
			return true;
		}

		Node& disconnect(InputPortIndex input)
		{
			assert(input.value() < NodeArgument::MaxNumInputs);
			m_inputs[input.value()].processor().r_consumers.find(this)->second.erase(input);
			m_inputs[input.value()] = Source{};
			touch();
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

		auto paramNames() const { return m_proc->paramNames(); }

		Node& set(ParamName param_name, ParamValue val)
		{
			m_proc->set(param_name, val);
			touch();
			return *this;
		}

		ParamValue get(ParamName param_name) const { return m_proc->get(param_name); }

		char const* name() const { return m_name.has_value() ? m_name->c_str() : m_proc->name(); }

		Node& userName(std::optional<std::string>&& val)
		{
			m_name = std::move(val);
			return *this;
		}

		Node& userName(std::optional<std::string> const& val)
		{
			m_name = std::move(val);
			return *this;
		}

		auto const& userName() const { return m_name; }

		auto processorId() const { return m_proc->id(); }

		auto nodeId() const { return m_id; }

		auto processorReleaseState() const { return m_proc->releaseState(); }

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

		void touch() { m_last_modified = now(); }

		size_t lastModified() const { return m_last_modified; }

		size_t lastRendered() const { return m_last_rendered; }

		DefaultRng::SeedValue rngSeed() const { return m_rng_seed; }

		Node& rngSeed(DefaultRng::SeedValue value)
		{
			m_rng_seed = value;
			touch();
			return *this;
		}

		auto resultCacheSize() const { return m_result_cache_size; }

	private:
		inline static std::atomic<size_t> s_clock;
		static size_t now() { return s_clock.fetch_add(1); }
		std::atomic<size_t> m_last_modified;
		mutable std::atomic<size_t> m_last_rendered;
		mutable Size2d m_result_cache_size;
		mutable result_type m_result_cache;

		std::array<Source, NodeArgument::MaxNumInputs> m_inputs;
		std::unique_ptr<AbstractImageProcessor> m_proc;
		NodeId m_id;
		DefaultRng::SeedValue m_rng_seed;
		std::optional<std::string> m_name;

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

	inline bool isUpToDate(Node const& node, Size2d size)
	{
		if(node.resultCacheSize() != size) { return false; }

		auto const inputs = node.inputs();
		if(std::size(inputs) != 0)
		{
			auto const& newest_input =
			    *std::ranges::max_element(inputs, [](auto const& a, auto const& b) {
				    auto const ta = a.processor().lastRendered();
				    auto const tb = b.processor().lastRendered();
				    return ta < tb;
			    });
			auto const t = newest_input.processor().lastRendered();
			return node.lastModified() < node.lastRendered() && t < node.lastRendered();
		}
		else
		{
			return node.lastModified() < node.lastRendered();
		}
	}

	inline bool isConnected(Node const& node)
	{
		return node.hasProcessor()
		       && std::ranges::none_of(node.inputs(), [](auto node) { return !node.valid(); });
	}

	bool isConnectedDeep(Node const& node);

	template<class Visitor>
	void visitEdges(Visitor&& visitor, Node const& node)
	{
		std::ranges::for_each(node.inputs(), std::forward<Visitor>(visitor));
	}

	inline auto reference(Node::Source const& src)
	{
		return src.valid() ? &src.processor() : nullptr;
	}

	std::map<std::string, double> params(Node const& node);

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
		DefaultRng::SeedValue rng_seed;
		std::optional<std::string> user_name;
		ImgProcReleaseState proc_relstate;
		std::array<NodeSourceData, NodeArgument::MaxNumInputs> inputs;
		std::map<std::string, double> params;
	};

	inline NodeData nodeData(Node const& node)
	{
		NodeData ret{node.processorId(),
		             node.rngSeed(),
		             node.userName(),
		             node.processorReleaseState(),
		             {},
		             params(node)};
		std::ranges::transform(node.inputs(), std::begin(ret.inputs), [](auto const& item) {
			if(item.valid()) { return NodeSourceData{item.processor().nodeId(), item.port()}; }
			return NodeSourceData{};
		});

		return ret;
	}

	inline void to_json(nlohmann::json& obj, NodeData const& node)
	{
		obj["imgproc"]  = node.imgproc;
		obj["rng_seed"] = node.rng_seed;
		if(node.user_name) { obj["user_name"] = *node.user_name; }
		obj["inputs"]        = node.inputs;
		obj["params"]        = node.params;
		obj["proc_relstate"] = node.proc_relstate;
	}

	inline void from_json(nlohmann::json const& obj, NodeData& node)
	{
		node.imgproc = obj.at("imgproc").get<ImageProcessorId>();

		node.rng_seed = DefaultRng::genSeed();
		if(auto i = obj.find("rng_seed"); i != std::end(obj))
		{ node.rng_seed = DefaultRng::SeedValue{*i}; }

		if(auto i = obj.find("user_name"); i != std::end(obj)) { node.user_name = std::string{*i}; }

		node.proc_relstate = ImgProcReleaseState::Stable;
		if(auto i = obj.find("proc_relstate"); i != std::end(obj))
		{ node.proc_relstate = ImgProcReleaseState{*i}; }

		auto inputs = obj.at("inputs").get<std::vector<NodeSourceData>>();
		if(std::size(inputs) > NodeArgument::MaxNumInputs)
		{ throw std::string{"Too many inputs for node"}; }


		std::ranges::copy(inputs, std::begin(node.inputs));
		node.params = obj.at("params").get<std::map<std::string, double>>();
	}
}

#endif
