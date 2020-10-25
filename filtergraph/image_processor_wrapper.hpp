//@	{
//@	 "targets":[{"name":"image_processor_wrapper.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_IMAGEPROCESSORWRAPPER_HPP
#define TEXPAINTER_FILTERGRAPH_IMAGEPROCESSORWRAPPER_HPP

#include "./node_argument.hpp"
#include "./port_info.hpp"
#include "./img_proc_arg.hpp"
#include "./abstract_image_processor.hpp"
#include "./image_processor.hpp"

#include "utils/memblock.hpp"

namespace Texpainter::FilterGraph
{
	namespace detail
	{
		template<class Buffers, size_t n = Buffers::size()>
		void alloc_output_buffers(Buffers& buffers, Size2d size)
		{
			if constexpr(n != 0)
			{
				buffers.template init<n - 1>(size);
				alloc_output_buffers<Buffers, n - 1>(buffers, size);
			}
		}

		template<class Buffers, class OutputArgs, size_t n = Buffers::size()>
		void get_output_buffers(Buffers& buffers, OutputArgs& args)
		{
			if constexpr(n != 0)
			{
				args.template get<n - 1>() = buffers.template get<n - 1>();
				get_output_buffers<Buffers, OutputArgs, n - 1>(buffers, args);
			}
		}

		template<class Buffers, size_t n = Buffers::size()>
		void take_output_buffers(Buffers& buffers, std::array<PortValue, 4>& ret)
		{
			if constexpr(n != 0)
			{
				ret[n - 1] = std::move(buffers.template take<n - 1>());
				take_output_buffers<Buffers, n - 1>(buffers, ret);
			}
		}
	}

	template<ImageProcessor Proc>
	class ImageProcessorWrapper final: public AbstractImageProcessor
	{
	public:
		explicit ImageProcessorWrapper(Proc&& proc): m_proc{std::move(proc)} {}

		result_type operator()(NodeArgument const& arg) const override
		{
			using InterfaceDescriptor = typename Proc::InterfaceDescriptor;

			using InputArgs  = typename ImgProcArg<InterfaceDescriptor>::InputArgs;
			using OutputArgs = typename ImgProcArg<InterfaceDescriptor>::OutputArgs;

			OutputBuffers<portTypes(InterfaceDescriptor::OutputPorts)> outputs;
			detail::alloc_output_buffers(outputs, arg.size());

			OutputArgs args_out;
			detail::get_output_buffers(outputs, args_out);

			m_proc(ImgProcArg<InterfaceDescriptor>{arg.size(), arg.inputs<InputArgs>(), args_out});

			result_type ret;
			detail::take_output_buffers(outputs, ret);
			return ret;
		}

		std::span<PortInfo const> inputPorts() const override
		{
			return Proc::InterfaceDescriptor::InputPorts;
		}

		std::span<PortInfo const> outputPorts() const override
		{
			return Proc::InterfaceDescriptor::OutputPorts;
		}

		std::span<ParamName const> paramNames() const override
		{
			return Proc::InterfaceDescriptor::ParamNames;
		}

		std::span<ParamValue const> paramValues() const override { return m_proc.paramValues(); }

		ParamValue get(ParamName param_name) const override { return m_proc.get(param_name); }

		AbstractImageProcessor& set(ParamName param_name, ParamValue value) override
		{
			m_proc.set(param_name, value);
			return *this;
		}

		std::unique_ptr<AbstractImageProcessor> clone() const override
		{
			return std::make_unique<ImageProcessorWrapper>(*this);
		}

		char const* name() const override { return Proc::name(); }

		ImageProcessorId id() const override { return Proc::id(); }

		Proc const& processor() const { return m_proc; }

		Proc& processor() { return m_proc; }

	private:
		Proc m_proc;
	};

	template<ImageProcessor Proc>
	std::unique_ptr<AbstractImageProcessor> createImageProcessor()
	{
		return std::make_unique<ImageProcessorWrapper<Proc>>(Proc{});
	}
}

#endif