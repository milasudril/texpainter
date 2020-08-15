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
	template<ImageProcessor Proc>
	class ImageProcessorWrapper
	{
	public:
		explicit ImageProcessorWrapper(Proc&& proc): m_proc{std::move(proc)} {}

		std::array<Memblock, 4> operator()(NodeArgument const& arg) const
		{
			std::array<Memblock, 4> ret{};
			std::ranges::transform(Proc::outputPorts(), ret, [size = arg.size()](auto port) {
				// FIXME: port is not constexpr
				auto elem_size = sizeof(PixelTypeToType<port.type>);
				return Memblock{size.area() * elem_size};
			});

			m_proc(ImgProcArg2<Proc>{arg, ret});

			return ret;
		}


	private:
		Proc m_proc;
	};
}

#endif