//@	{
//@	 "targets":[{"name":"image_processor_wrapper.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_IMAGEPROCESSORWRAPPER_HPP
#define TEXPAINTER_FILTERGRAPH_IMAGEPROCESSORWRAPPER_HPP

#include "./node_argument.hpp"
#include "./proctypes.hpp"
#include "./img_proc_arg.hpp"
#include "./abstract_image_processor.hpp"
#include "./image_processor.hpp"

#include "utils/memblock.hpp"

namespace Texpainter::FilterGraph
{
	namespace detail
	{
		template<auto outputs, size_t n = std::size(outputs)>
		void get_sizes(auto& size_array)
		{
			if constexpr(n != 0)
			{
				size_array[n - 1] = sizeof(typename PixelTypeToType<outputs[n - 1]>::type);
				get_sizes<outputs, n - 1>(size_array);
			}
		}

		template<class InterfaceDescriptor>
		auto alloc_output_buffers(Size2d size)
		{
			static_assert(std::size(InterfaceDescriptor::OutputPorts) <= 4);
			std::array<size_t, std::size(InterfaceDescriptor::OutputPorts)> sizes;
			get_sizes<portTypes(InterfaceDescriptor::OutputPorts)>(sizes);
			std::array<Memblock, 4> ret;
			std::ranges::transform(
			    sizes, std::begin(ret), [size](auto val) { return Memblock{size.area() * val}; });
			return ret;
		}

		template<class T>
		void do_cast(T& pointer, void* other)
		{
			pointer = reinterpret_cast<std::decay_t<T>>(other);
		}
	}

	template<ImageProcessor Proc>
	class ImageProcessorWrapper final: public AbstractImageProcessor
	{
	public:
		explicit ImageProcessorWrapper(Proc&& proc): m_proc{std::move(proc)} {}

		std::array<Memblock, 4> operator()(NodeArgument const& arg) const override
		{
			using InterfaceDescriptor = typename Proc::InterfaceDescriptor;
			auto ret = detail::alloc_output_buffers<InterfaceDescriptor>(arg.size());

			using InputArgs  = typename ImgProcArg<InterfaceDescriptor>::InputArgs;
			using OutputArgs = typename ImgProcArg<InterfaceDescriptor>::OutputArgs;

			OutputArgs args_out{};
			detail::apply(
			    [&ret, index = 0](auto&... args) mutable {
				    (..., detail::do_cast(args, ret[index++].get()));
			    },
			    args_out);

			m_proc(ImgProcArg<InterfaceDescriptor>{arg.size(), arg.inputs<InputArgs>(), args_out});

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

		Proc const& processor() const { return m_proc; }

		Proc& processor() { return m_proc; }

	private:
		Proc m_proc;
	};

	template<ImageProcessor Proc>
	decltype(auto) createProcInstance()
	{
		return std::make_unique<ImageProcessorWrapper<Proc>>(Proc{});
	}
}

#endif