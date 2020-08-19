//@	{
//@	 "targets":[{"name":"image_processor.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_IMAGEPROCESSOR_HPP
#define TEXPAINTER_FILTERGRAPH_IMAGEPROCESSOR_HPP

#include "./img_proc_arg.hpp"

#include "utils/memblock.hpp"

namespace Texpainter::FilterGraph
{
	template<class T>
	concept ImageProcessor2 = requires(T a)
	{
		{
			std::as_const(a)(std::declval<ImgProcArg2<typename T::InterfaceDescriptor>>())
		}
		->std::same_as<void>;

		{
			T::name()
		}
		->std::same_as<char const*>;
	};
}

#endif