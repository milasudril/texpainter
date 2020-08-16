//@	{
//@	 "targets":[{"name":"node.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"node.o", "rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_NODE_HPP
#define TEXPAINTER_FILTERGRAPH_NODE_HPP

#include "./proctypes.hpp"

#include "pixel_store/image.hpp"

#include <complex>
#include <vector>
#include <variant>
#include <map>
#include <set>
#include <memory>

namespace Texpainter::FilterGraph
{
	class Node
	{
	public:
		using argument_type = ImgProcArg;
		using result_type   = ImgProcRetval;

		class AbstractImageProcessor
		{
		public:
			virtual std::vector<result_type> operator()(std::span<argument_type const>) const  = 0;
			virtual std::span<PortInfo const> inputPorts() const                               = 0;
			virtual std::span<PortInfo const> outputPorts() const                              = 0;
			virtual std::span<char const* const> paramNames() const                            = 0;
			virtual std::span<ParamValue const> paramValues() const                            = 0;
			virtual ParamValue get(std::string_view param_name) const                          = 0;
			virtual AbstractImageProcessor& set(std::string_view param_name, ParamValue value) = 0;
			virtual std::unique_ptr<AbstractImageProcessor> clone() const                      = 0;
			virtual char const* name() const                                                   = 0;
			virtual ~AbstractImageProcessor() = default;
		};

		class SourceNode
		{
		public:
			SourceNode(): r_processor{s_default_node}, m_index{OutputPort{0}} {}

			explicit SourceNode(std::reference_wrapper<Node const> node, OutputPort index)
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

			Node const& processor() const { return r_processor.get(); }

			OutputPort port() const { return m_index; }

		private:
			std::reference_wrapper<Node const> r_processor;
			OutputPort m_index;
		};

		explicit Node(std::unique_ptr<AbstractImageProcessor>&& proc)
		    : m_inputs(proc->inputPorts().size())
		    , m_proc{std::move(proc)}
		{
		}

		Node(Node&&) = delete;

		Node(): m_proc{std::make_unique<DummyImageProcessor>()} {}

		template<class Proc, std::enable_if_t<!std::is_same_v<Node, std::decay_t<Proc>>, int> = 0>
		explicit Node(Proc&& proc)
		    : m_inputs(
		        proc.inputPorts().size())  // Must use old-style ctor here to get the correct size
		    , m_proc{std::make_unique<ImageProcessorWrapper<std::decay_t<Proc>>>(
		          std::forward<Proc>(proc))}
		{
		}

		auto disconnectedCopy() const { return m_proc->clone(); }

		template<class Proc, std::enable_if_t<!std::is_same_v<Node, std::decay_t<Proc>>, int> = 0>
		Node& replaceWith(Proc&& proc)
		{
			m_inputs = std::vector<SourceNode>(proc.inputPorts().size());
			m_result_cache.clear();
			m_proc = std::make_unique<ImageProcessorWrapper<std::decay_t<Proc>>>(
			    std::forward<Proc>(proc));
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

		Node& connect(InputPort input, std::reference_wrapper<Node const> other, OutputPort output)
		{
			other.get().r_consumers[this].insert(input);
			m_inputs[input.value()] = SourceNode{other, output};
			m_result_cache.clear();
			return *this;
		}

		Node& disconnect(InputPort input)
		{
			m_inputs[input.value()].processor().r_consumers.find(this)->second.erase(input);
			m_inputs[input.value()] = SourceNode{};
			m_result_cache.clear();
			return *this;
		}

		std::span<SourceNode const> inputs() const { return m_inputs; }

		auto paramNames() const { return m_proc->paramNames(); }

		auto paramValues() const { return m_proc->paramValues(); }

		Node& set(std::string_view param_name, ParamValue val)
		{
			m_proc->set(param_name, val);
			m_result_cache.clear();
			return *this;
		}

		ParamValue get(std::string_view param_name) const { return m_proc->get(param_name); }

		auto name() const { return m_proc->name(); }

		~Node()
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
		static Node s_default_node;

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
		mutable std::map<Node*, std::set<InputPort, InputPortCompare>> r_consumers;

		std::unique_ptr<AbstractImageProcessor> m_proc;

		template<ImageProcessor Proc>
		class ImageProcessorWrapper: public AbstractImageProcessor
		{
		public:
			explicit ImageProcessorWrapper(Proc&& proc): m_proc{std::move(proc)} {}

			std::vector<result_type> operator()(std::span<argument_type const> args) const override
			{
				return m_proc(args);
			}

			std::span<PortInfo const> inputPorts() const override { return m_proc.inputPorts(); }

			std::span<PortInfo const> outputPorts() const override { return m_proc.outputPorts(); }

			std::span<char const* const> paramNames() const override { return m_proc.paramNames(); }

			std::span<ParamValue const> paramValues() const override
			{
				return m_proc.paramValues();
			}

			ParamValue get(std::string_view param_name) const override
			{
				return m_proc.get(param_name);
			}

			ImageProcessorWrapper& set(std::string_view param_name, ParamValue value) override
			{
				m_proc.set(param_name, value);
				return *this;
			}

			std::unique_ptr<AbstractImageProcessor> clone() const override
			{
				return std::make_unique<ImageProcessorWrapper>(*this);
			}

			char const* name() const override { return Proc::name(); }


		private:
			Proc m_proc;
		};

		class DummyImageProcessor: public AbstractImageProcessor
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

			std::span<ParamValue const> paramValues() const override
			{
				return std::span<ParamValue const>{};
			}

			ParamValue get(std::string_view) const override { return ParamValue{0.0}; }

			DummyImageProcessor& set(std::string_view, ParamValue) override { return *this; }

			std::unique_ptr<AbstractImageProcessor> clone() const override
			{
				return std::make_unique<DummyImageProcessor>(*this);
			}

			char const* name() const override { return "*empty*"; }
		};
	};
}

#endif