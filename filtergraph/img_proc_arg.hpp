//@	{
//@	 "targets":[{"name":"img_proc_arg.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_IMGPROCARG_HPP
#define TEXPAINTER_FILTERGRAPH_IMGPROCARG_HPP

#include "./argtuple.hpp"
#include "./img_proc_interface_descriptor.hpp"

#include "utils/size_2d.hpp"

#include <functional>

namespace Texpainter::FilterGraph
{
	template<ImgProcInterfaceDescriptor Descriptor>
	class ImgProcArg
	{
	public:
		using InputArgs  = InArgTuple<portTypes(Descriptor::InputPorts)>;
		using OutputArgs = OutArgTuple<portTypes(Descriptor::OutputPorts)>;

		ImgProcArg(): m_size{0, 0} {}

		explicit ImgProcArg(Size2d size, InputArgs const& inputs, OutputArgs const& outputs)
		    : m_size{size}
		    , m_inputs{inputs}
		    , m_outputs{outputs}
		{
		}

		Size2d size() const { return m_size; }

		template<size_t index>
		auto input(uint32_t col, uint32_t row) const
		{
			return std::get<index>(m_inputs)[row * m_size.width() + col];
		}

		template<size_t index>
		auto input() const
		{
			return std::get<index>(m_inputs);
		}

		template<size_t index>
		auto& output(uint32_t col, uint32_t row) const
		{
			return std::get<index>(m_outputs)[row * m_size.width() + col];
		}

		template<size_t index>
		auto output() const
		{
			return std::get<index>(m_outputs);
		}

	private:
		Size2d m_size;
		InputArgs m_inputs;
		OutputArgs m_outputs;
	};
}

#endif