//@	{
//@	 "targets":[{"name":"img_proc_arg.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_IMGPROCARG_HPP
#define TEXPAINTER_FILTERGRAPH_IMGPROCARG_HPP

#include "./argtuple.hpp"
#include "./proctypes.hpp"
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
			return m_inputs.template get<index>()[row * m_size.width() + col];
		}

		template<size_t index>
		auto input() const
		{
			return m_inputs.template get<index>();
		}

		template<size_t index>
		auto& output(uint32_t col, uint32_t row) const
		{
			return m_outputs.template get<index>()[row * m_size.width() + col];
		}

		template<size_t index>
		auto output() const
		{
			return m_outputs.template get<index>();
		}

	private:
		Size2d m_size;
		InputArgs m_inputs;
		OutputArgs m_outputs;
	};
}

#endif