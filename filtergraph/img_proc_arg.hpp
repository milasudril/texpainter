//@	{
//@	 "targets":[{"name":"img_proc_arg.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_IMG_PROC_ARG_HPP
#define TEXPAINTER_FILTERGRAPH_IMG_PROC_ARG_HPP

#include "./argtuple.hpp"
#include "./node_argument.hpp"
#include "./proctypes.hpp"

#include "utils/size_2d.hpp"

#include <array>
#include <functional>

namespace Texpainter::FilterGraph
{
	namespace detail
	{
		template<class PortArray>
		constexpr auto port_types(PortArray ports)
		{
			std::array<PixelType, ports.size()> ret{};
			std::ranges::transform(ports, std::begin(ret),  [](auto val){ return val.type; });
			return ret;
		}
	}

	template<ImageProcessor ImgProc>
	class ImgProcArg2
	{
	public:
		explicit ImgProcArg2(NodeArgument const& node_args)
		    : m_size{node_args.size()}
		    , m_inputs{node_args.inputs<std::decay_t<decltype(m_inputs)>>()}
		{
		}

		Size2d size() const { return m_size; }

		template<size_t index>
		auto input(uint32_t col, uint32_t row) const
		{
			return m_inputs.template get<index>()[row * m_size.width() + col];
		}

		template<size_t index>
		auto input() const
		{
			return m_inputs.template get<index>();
		}

	private:
		Size2d m_size;
		InArgTuple<detail::port_types(ImgProc::inputPorts())> m_inputs;
		OutArgTuple<detail::port_types(ImgProc::outputPorts())> m_outputs;
	};
}

#endif