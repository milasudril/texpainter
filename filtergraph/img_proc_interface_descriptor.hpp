//@	{
//@	 "targets":[{"name":"img_proc_interface_descriptor.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_IMGPROCINTERFACEDESCRIPTOR_HPP
#define TEXPAINTER_FILTERGRAPH_IMGPROCINTERFACEDESCRIPTOR_HPP

#include "./port_info.hpp"
#include "./img_proc_param.hpp"

#include <concepts>
#include <type_traits>
#include <span>

namespace Texpainter::FilterGraph
{
	template<class T>
	concept ImgProcInterfaceDescriptor = requires(T a)
	{
		{
			std::decay_t<T>::InputPorts
		}
		->std::convertible_to<std::span<PortInfo const>>;
		{
			std::decay_t<T>::OutputPorts
		}
		->std::convertible_to<std::span<PortInfo const>>;
		{
			std::decay_t<T>::ParamNames
		}
		->std::convertible_to<std::span<ParamName const>>;
	};
}

#endif