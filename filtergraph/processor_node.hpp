//@	{
//@	 "targets":[{"name":"processor_node.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"processor_node.o", "rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_PROCESSORNODE_HPP
#define TEXPAINTER_FILTERGRAPH_PROCESSORNODE_HPP

#include "./proctypes.hpp"

#include "pixel_store/image.hpp"

#include <complex>
#include <vector>
#include <variant>
#include <map>
#include <set>

namespace Texpainter::FilterGraph
{
	class ProcessorNode
	{
	public:
		using argument_type = ProcArgumentType;
		using result_type   = ProcResultType;

		class Processor
		{
		public:
			virtual std::vector<result_type> operator()(std::span<argument_type const>) const = 0;
			virtual std::span<PortInfo const> inputPorts() const                              = 0;
			virtual std::span<PortInfo const> outputPorts() const                             = 0;
			virtual std::span<char const* const> paramNames() const                           = 0;
			virtual std::span<ProcParamValue const> paramValues() const                       = 0;
			virtual ProcParamValue get(std::string_view param_name) const                     = 0;
			virtual Processor& set(std::string_view param_name, ProcParamValue value)         = 0;
			virtual std::unique_ptr<Processor> clone() const                                  = 0;
			virtual char const* name() const                                                  = 0;
			virtual ~Processor() = default;
		};

		class SourceNode
		{
		public:
			SourceNode(): r_processor{s_default_node}, m_index{OutputPort{0}} {}

			explicit SourceNode(std::reference_wrapper<ProcessorNode const> node, OutputPort index)
			    : r_processor{node}
			    , m_index{index}
			{
			}

			argument_type operator()() const
			{
				if(auto const& res = r_processor(); m_index.value() < std::size(res)) [[likely]]
					{
						return std::visit(
						    [](auto const& val) { return argument_type{val.pixels()}; },
						    res[m_index.value()]);
					}

				return argument_type{};
			}

			ProcessorNode const& processor() const { return r_processor.get(); }

			OutputPort port() const { return m_index; }

		private:
			std::reference_wrapper<ProcessorNode const> r_processor;
			OutputPort m_index;
		};

		explicit ProcessorNode(std::unique_ptr<Processor>&& proc)
		    : m_inputs(proc->inputPorts().size())
		    , m_proc{std::move(proc)}
		{
		}

		ProcessorNode(ProcessorNode&&) = delete;

		ProcessorNode(): m_proc{std::make_unique<ProcessorDummy>()} {}

		template<class Proc,
		         std::enable_if_t<!std::is_same_v<ProcessorNode, std::decay_t<Proc>>, int> = 0>
		explicit ProcessorNode(Proc&& proc)
		    : m_inputs(
		        proc.inputPorts().size())  // Must use old-style ctor here to get the correct size
		    , m_proc{std::make_unique<ProcessorImpl<std::decay_t<Proc>>>(std::forward<Proc>(proc))}
		{
		}

		auto disconnectedCopy() const { return m_proc->clone(); }

		template<class Proc,
		         std::enable_if_t<!std::is_same_v<ProcessorNode, std::decay_t<Proc>>, int> = 0>
		ProcessorNode& replaceWith(Proc&& proc)
		{
			m_inputs = std::vector<SourceNode>(proc.inputPorts().size());
			m_result_cache.clear();
			m_proc = std::make_unique<ProcessorImpl<std::decay_t<Proc>>>(std::forward<Proc>(proc));
			return *this;
		}

		bool dirty() const
		{
			return m_result_cache.size() == 0
			       || std::ranges::any_of(
			           m_inputs, [](auto const& item) { return item.processor().dirty(); });
		}


		std::vector<result_type> const& operator()() const
		{
			if(!dirty()) { return m_result_cache; }

			std::vector<argument_type> args;
			std::ranges::transform(m_inputs, std::back_inserter(args), [](auto const& val) {
				auto const& ret = val();
				return ret;
			});
			m_result_cache = (*m_proc)(args);
			return m_result_cache;
		}

		auto inputPorts() const { return m_proc->inputPorts(); }

		auto outputPorts() const { return m_proc->outputPorts(); }

		ProcessorNode& connect(InputPort input,
		                       std::reference_wrapper<ProcessorNode const> other,
		                       OutputPort output)
		{
			other.get().r_consumers[this].insert(input);
			m_inputs[input.value()] = SourceNode{other, output};
			m_result_cache.clear();
			return *this;
		}

		ProcessorNode& disconnect(InputPort input)
		{
			m_inputs[input.value()].processor().r_consumers.find(this)->second.erase(input);
			m_inputs[input.value()] = SourceNode{};
			m_result_cache.clear();
			return *this;
		}

		std::span<SourceNode const> inputs() const { return m_inputs; }

		auto paramNames() const { return m_proc->paramNames(); }

		auto paramValues() const { return m_proc->paramValues(); }

		ProcessorNode& set(std::string_view param_name, ProcParamValue val)
		{
			m_proc->set(param_name, val);
			m_result_cache.clear();
			return *this;
		}

		ProcParamValue get(std::string_view param_name) const { return m_proc->get(param_name); }

		auto name() const { return m_proc->name(); }

		~ProcessorNode()
		{
			std::ranges::for_each(r_consumers, [](auto const& item) {
				std::ranges::for_each(item.second, [proc = item.first](auto port) {
					// NOTE: Do not call disconnect here. This would screw up iterators.
					//       Also, r_consumers will be destroyed anywas.
					proc->m_inputs[port.value()] = SourceNode{};
				});
			});

			// NOTE: Remember to also disconnect all inputs. Otherwise, there will
			//       be dead pointers left in the producer.
			for(size_t k = 0; k < m_inputs.size(); ++k)
			{
				disconnect(InputPort{static_cast<uint32_t>(k)});
			}
		}

	private:
		mutable std::vector<result_type> m_result_cache;
		static ProcessorNode s_default_node;

		std::vector<SourceNode> m_inputs;

		struct InputPortCompare
		{
			constexpr bool operator()(InputPort a, InputPort b) const
			{
				return a.value() < b.value();
			}
		};

		// NOTE: Using mutable here is not a very good solution, but allows treating producer nodes
		//       as const from the consumer side.
		mutable std::map<ProcessorNode*, std::set<InputPort, InputPortCompare>> r_consumers;

		std::unique_ptr<Processor> m_proc;

		template<ImageProcessor Proc>
		class ProcessorImpl: public Processor
		{
		public:
			explicit ProcessorImpl(Proc&& proc): m_proc{std::move(proc)} {}

			std::vector<result_type> operator()(std::span<argument_type const> args) const override
			{
				return m_proc(args);
			}

			std::span<PortInfo const> inputPorts() const override { return m_proc.inputPorts(); }

			std::span<PortInfo const> outputPorts() const override { return m_proc.outputPorts(); }

			std::span<char const* const> paramNames() const override { return m_proc.paramNames(); }

			std::span<ProcParamValue const> paramValues() const override
			{
				return m_proc.paramValues();
			}

			ProcParamValue get(std::string_view param_name) const override
			{
				return m_proc.get(param_name);
			}

			ProcessorImpl& set(std::string_view param_name, ProcParamValue value) override
			{
				m_proc.set(param_name, value);
				return *this;
			}

			std::unique_ptr<Processor> clone() const override
			{
				return std::make_unique<ProcessorImpl>(*this);
			}

			char const* name() const override { return Proc::name(); }


		private:
			Proc m_proc;
		};

		class ProcessorDummy: public Processor
		{
		public:
			std::vector<result_type> operator()(std::span<argument_type const> args) const
			{
				return std::vector<result_type>{};
			}

			std::span<PortInfo const> inputPorts() const override
			{
				return std::span<PortInfo const>{};
			}

			std::span<PortInfo const> outputPorts() const override
			{
				return std::span<PortInfo const>{};
			}

			std::span<char const* const> paramNames() const override
			{
				return std::span<char const* const>{};
			}

			std::span<ProcParamValue const> paramValues() const override
			{
				return std::span<ProcParamValue const>{};
			}

			ProcParamValue get(std::string_view) const override { return ProcParamValue{0.0}; }

			ProcessorDummy& set(std::string_view, ProcParamValue) override { return *this; }

			std::unique_ptr<Processor> clone() const override
			{
				return std::make_unique<ProcessorDummy>(*this);
			}

			char const* name() const override { return "*empty*"; }
		};
	};
}

#endif