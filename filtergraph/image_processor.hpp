//@	{
//@	 "targets":[{"name":"image_processor.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_IMAGEPROCESSOR_HPP
#define TEXPAINTER_FILTERGRAPH_IMAGEPROCESSOR_HPP

#include "./img_proc_arg.hpp"
#include "./img_proc_interface_descriptor.hpp"

#include "utils/memblock.hpp"

#include <concepts>

namespace Texpainter::FilterGraph
{
	template<class T>
	concept ImageProcessor2 = requires(T a)
	{
		{std::as_const(a)(std::declval<ImgProcArg2<typename T::InterfaceDescriptor>>())};

		{
			std::declval<typename T::InterfaceDescriptor>()
		}
		->ImgProcInterfaceDescriptor;

		{
			std::as_const(a).paramValues()
		}
		->std::convertible_to<std::span<ParamValue const>>;

		{
			std::as_const(a).get(std::declval<ParamName>())
		}
		->std::same_as<ParamValue>;

		{a.set(std::declval<ParamName>(), std::declval<ParamValue>())};

		{
			T::name()
		}
		->std::same_as<char const*>;
	}
	&&std::copy_constructible<T>;
}

#endif