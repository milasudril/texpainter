//@	{
//@	 "targets":[{"name":"node.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_NODE_HPP
#define TEXPAINTER_FILTERGRAPH_NODE_HPP

#include "./abstract_image_processor.hpp"

#include <map>
#include <set>
#include <cassert>

namespace Texpainter::FilterGraph
{
	class Node;

	inline bool isConnected(Node const& node);

	class Node
	{
		class SourceNode
		{
		public:
			SourceNode(): r_processor{nullptr}, m_index{OutputPortIndex{0}} {}

			explicit SourceNode(Node const* node, OutputPortIndex index)
			    : m_last_usecount{0}
			    , r_processor{node}
			    , m_index{index}
			{
			}

			auto const& operator()(Size2d size) const
			{
				assert(valid());
				auto const& ret = (*r_processor)(size);
				m_last_usecount = r_processor->m_usecount;
				return ret[m_index.value()];
			}

			Node const& processor() const { return *r_processor; }

			OutputPortIndex port() const { return m_index; }

			bool valid() const { return r_processor != nullptr; }

			bool hasOldResult() const { return m_last_usecount < r_processor->m_usecount; }

		private:
			mutable size_t m_last_usecount;
			Node const* r_processor;
			OutputPortIndex m_index;
		};

	public:
		using result_type = AbstractImageProcessor::result_type;


		explicit Node(std::unique_ptr<AbstractImageProcessor> proc)
		    : m_dirty{1}
		    , m_usecount{static_cast<size_t>(-1)}
		    , m_proc{std::move(proc)}
		{
		}

		Node(Node&&) = delete;

		Node(): m_dirty{0}, m_proc{nullptr} {}

		auto clonedProcessor() const { return m_proc->clone(); }

		AbstractImageProcessor const& processor() const { return *m_proc.get(); }

		AbstractImageProcessor& processor() { return *m_proc.get(); }

		bool hasProcessor() const { return m_proc != nullptr; }

		auto inputPorts() const { return m_proc->inputPorts(); }

		auto outputPorts() const { return m_proc->outputPorts(); }

		void forceUpdate() const { m_dirty = true; }

		result_type const& operator()(Size2d size) const
		{
			assert(FilterGraph::isConnected(*this));

			if(!dirty()) { return m_result_cache; }

			std::array<InputPortValue, NodeArgument::MaxNumInputs> args{};
			auto const n_ports   = inputPorts().size();
			auto const input_end = std::begin(m_inputs) + n_ports;
			std::transform(std::begin(m_inputs),
			               input_end,
			               std::begin(args),
			               [size](auto const& val) { return makeInputPortValue(val(size)); });

			m_result_cache = (*m_proc)(NodeArgument{size, args});
			++m_usecount;
			m_dirty = 0;
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

			m_inputs[input.value()] = SourceNode{&other.get(), output};
			clear_result_cache();
			return *this;
		}

		Node& disconnect(InputPortIndex input)
		{
			assert(input.value() < NodeArgument::MaxNumInputs);
			m_inputs[input.value()].processor().r_consumers.find(this)->second.erase(input);
			m_inputs[input.value()] = SourceNode{};
			clear_result_cache();
			return *this;
		}

		bool isConnected(InputPortIndex input) const
		{
			assert(input.value() < NodeArgument::MaxNumInputs);
			return m_inputs[input.value()].valid();
		}

		std::span<SourceNode const> inputs() const
		{
			return {std::begin(m_inputs), std::begin(m_inputs) + inputPorts().size()};
		}

		auto paramNames() const { return m_proc->paramNames(); }

		auto paramValues() const { return m_proc->paramValues(); }

		Node& set(ParamName param_name, ParamValue val)
		{
			m_proc->set(param_name, val);
			clear_result_cache();
			return *this;
		}

		ParamValue get(ParamName param_name) const { return m_proc->get(param_name); }

		char const* name() const { return m_proc->name(); }

		~Node()
		{
			std::ranges::for_each(r_consumers, [](auto const& item) {
				std::ranges::for_each(item.second, [proc = item.first](auto port) {
					// NOTE: Do not call disconnect here. This would screw up iterators.
					//       Also, r_consumers will be destroyed anywas.
					if(proc != nullptr) { proc->m_inputs[port.value()] = SourceNode{}; }
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
		mutable size_t m_dirty;
		mutable size_t m_usecount;
		std::array<SourceNode, NodeArgument::MaxNumInputs> m_inputs;
		std::unique_ptr<AbstractImageProcessor> m_proc;
		mutable result_type m_result_cache;

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


		void clear_result_cache()
		{
			m_dirty        = 1;
			m_result_cache = result_type{};
		}

		bool dirty() const
		{
			return m_dirty || std::ranges::any_of(inputs(), [](auto const& item) {
				       return item.processor().dirty() || item.hasOldResult();
			       });
		}
	};

	inline bool isConnected(Node const& node)
	{
		return node.hasProcessor()
		       && std::ranges::none_of(node.inputs(), [](auto node) { return !node.valid(); });
	}
}

#endif