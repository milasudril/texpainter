//@	{
//@	 "targets":[{"name":"processor_node.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"processor_node.o", "rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_FILTERPIPE_PROCESSORNODE_HPP
#define TEXPAINTER_FILTERPIPE_PROCESSORNODE_HPP

#include "model/image.hpp"

#include <complex>
#include <vector>
#include <variant>

namespace Texpainter::FilterGraph
{
	struct ProcParamValue
	{
		double m_value;

		constexpr auto operator<=>(ProcParamValue const&) const = default;
	};


	enum class PixelType : size_t
	{
		RGBA,
		GrayscaleReal,
		GrayscaleComplex
	};

	using ProcArgumentType = std::variant<Span2d<Model::Pixel const>,
	                                      Span2d<double const>,
	                                      Span2d<std::complex<double> const>>;

	inline PixelType pixelType(ProcArgumentType const& x)
	{
		return static_cast<PixelType>(x.index());
	}

	using ProcResultType = std::variant<Model::BasicImage<Model::Pixel>,
	                                    Model::BasicImage<double>,
	                                    Model::BasicImage<std::complex<double>>>;

	inline PixelType pixelType(ProcResultType const& x)
	{
		return static_cast<PixelType>(x.index());
	}

	class ProcessorNode
	{
	public:
		using argument_type = ProcArgumentType;

		using result_type = ProcResultType;


		ProcessorNode(): m_proc{std::make_unique<ProcessorDummy>()} {}

		template<class Proc,
		         std::enable_if_t<!std::is_same_v<ProcessorNode, std::decay_t<Proc>>, int> = 0>
		explicit ProcessorNode(Proc&& proc)
		    : m_inputs(proc.inputCount())  // Must use old-style ctor here to get the correct size
		    , m_proc{std::make_unique<ProcessorImpl<std::decay_t<Proc>>>(std::forward<Proc>(proc))}
		{
		}


		std::vector<result_type> const& operator()() const
		{
			if(m_result_cache.size() != 0) { return m_result_cache; }

			std::vector<argument_type> args;
			std::ranges::transform(m_inputs, std::back_inserter(args), [](auto const& val) {
				auto const& ret = val();
				return ret;
			});
			m_result_cache = (*m_proc)(args);
			return m_result_cache;
		}


		ProcessorNode& connect(size_t socket,
		                       std::reference_wrapper<ProcessorNode const> other,
		                       size_t other_socket)
		{
			m_inputs[socket] = SourceNode{other, other_socket};
			return *this;
		}

		ProcessorNode& disconnect(size_t socket)
		{
			m_inputs[socket] = SourceNode{};
			return *this;
		}

		size_t inputCount() const { return m_inputs.size(); }


		ProcessorNode& set(std::string_view param_name, ProcParamValue val)
		{
			m_proc->set(param_name, val);
			return *this;
		}

		ProcParamValue get(std::string_view param_name) const { return m_proc->get(param_name); }


	private:
		mutable std::vector<result_type> m_result_cache;
		static ProcessorNode s_default_node;

		class SourceNode
		{
		public:
			SourceNode(): r_processor{s_default_node}, m_index{0} {}

			explicit SourceNode(std::reference_wrapper<ProcessorNode const> node, size_t index)
			    : r_processor{node}
			    , m_index{index}
			{
			}

			argument_type operator()() const
			{
				if(auto const& res = r_processor(); m_index < std::size(res)) [[likely]]
					{
						return std::visit(
						    [](auto const& val) {
							    return argument_type{val.pixels()};
						    },
						    res[m_index]);
					}

				return argument_type{};
			}

		private:
			std::reference_wrapper<ProcessorNode const> r_processor;
			size_t m_index;
		};

		std::vector<SourceNode> m_inputs;

		class Processor
		{
		public:
			virtual std::vector<result_type> operator()(std::span<argument_type const>) const = 0;
			virtual ProcParamValue get(std::string_view param_name) const                     = 0;
			virtual Processor& set(std::string_view param_name, ProcParamValue value)         = 0;
			virtual ~Processor() = default;
		};
		std::unique_ptr<Processor> m_proc;


		template<class Proc>
		class ProcessorImpl: public Processor
		{
		public:
			template<class T,
			         std::enable_if_t<!std::is_same_v<ProcessorImpl, std::decay_t<T>>, int> = 0>
			explicit ProcessorImpl(T&& proc): m_proc{std::move(proc)}
			{
			}

			std::vector<result_type> operator()(std::span<argument_type const> args) const override
			{
				return m_proc(args);
			}

			ProcParamValue get(std::string_view param_name) const override
			{
				return m_proc.get(param_name);
			}

			ProcessorImpl& set(std::string_view param_name, ProcParamValue value) override
			{
				(void)m_proc.set(param_name, value);
				return *this;
			}

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

			ProcParamValue get(std::string_view) const override { return ProcParamValue{0.0}; }

			ProcessorDummy& set(std::string_view, ProcParamValue) override { return *this; }
		};
	};
}

#endif