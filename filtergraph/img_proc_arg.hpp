//@	{
//@	 "targets":[{"name":"img_proc_arg.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_IMG_PROC_ARG_HPP
#define TEXPAINTER_FILTERGRAPH_IMG_PROC_ARG_HPP

#include "./argtuple.hpp"
#include "./proctypes.hpp"

#include "utils/size_2d.hpp"

#include <functional>

namespace Texpainter::FilterGraph
{
	template<ImageProcessor ImgProc>
	class ImgProcArg2
	{
	public:
		explicit ImgProcArg2(Size2d size,
		                     InArgTuple<portTypes(ImgProc::inputPorts())> const& inputs,
		                     OutArgTuple<portTypes(ImgProc::outputPorts())> const& outputs)
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
		InArgTuple<portTypes(ImgProc::inputPorts())> m_inputs;
		OutArgTuple<portTypes(ImgProc::outputPorts())> m_outputs;
	};
}

#endif