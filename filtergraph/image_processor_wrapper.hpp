//@	{
//@	 "targets":[{"name":"imageprocessorwrapper.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_IMAGEPROCESSORWRAPPER_HPP
#define TEXPAINTER_FILTERGRAPH_IMAGEPROCESSORWRAPPER_HPP

#include "./node_argument.hpp"
#include "./proctypes.hpp"
#include "./img_proc_arg.hpp"

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

		template<ImageProcessor ImgProc>
		auto alloc_output_buffers(Size2d size)
		{
			static_assert(std::size(ImgProc::outputPorts()) <= 4);
			std::array<size_t, std::size(ImgProc::outputPorts())> sizes;
			get_sizes<portTypes(ImgProc::outputPorts())>(sizes);
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
	class ImageProcessorWrapper
	{
	public:
		explicit ImageProcessorWrapper(Proc&& proc): m_proc{std::move(proc)} {}

		std::array<Memblock, 4> operator()(NodeArgument const& arg) const
		{
			auto ret = detail::alloc_output_buffers<Proc>(arg.size());

			using InputArgs  = typename ImgProcArg2<Proc>::InputArgs;
			using OutputArgs = typename ImgProcArg2<Proc>::OutputArgs;

			OutputArgs args_out{};
			detail::apply(
			    [&ret, index = 0](auto&... args) mutable {
				    (..., detail::do_cast(args, ret[index++].get()));
			    },
			    args_out);

			m_proc(ImgProcArg2<Proc>{arg.size(), arg.inputs<InputArgs>(), args_out});

			return ret;
		}

	private:
		Proc m_proc;
	};
}

#endif