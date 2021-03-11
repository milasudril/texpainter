//@	{
//@	 "targets":[{"name":"image_processor.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_IMAGEPROCESSOR_HPP
#define TEXPAINTER_FILTERGRAPH_IMAGEPROCESSOR_HPP

#include "./img_proc_arg.hpp"
#include "./image_processor_id.hpp"
#include "./img_proc_interface_descriptor.hpp"

#include <concepts>

namespace Texpainter::FilterGraph
{
	template<class T>
	concept ImageProcessor = requires(T a)
	{
		{std::as_const(a)(std::declval<ImgProcArg<typename T::InterfaceDescriptor>>())};

		{
			std::declval<typename T::InterfaceDescriptor>()
		}
		->ImgProcInterfaceDescriptor;

		{
			std::as_const(a).get(std::declval<ParamName>())
		}
		->std::same_as<ParamValue>;

		{a.set(std::declval<ParamName>(), std::declval<ParamValue>())};

		{
			std::as_const(a).name()
		}
		->std::same_as<char const*>;

		{
			T::id()
		}
		->std::same_as<ImageProcessorId>;

		{
			T::releaseState()
		}
		->std::same_as<ImgProcReleaseState>;
	}
	&&std::copy_constructible<T>;
}

#endif