//@	{
//@	 "targets":[{"name":"node_argument.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_NODEARGUMENT_HPP
#define TEXPAINTER_FILTERGRAPH_NODEARGUMENT_HPP

#include "./port_value.hpp"

#include "utils/size_2d.hpp"

#include <array>
#include <functional>

namespace Texpainter::FilterGraph
{
	class NodeArgument
	{
	public:
		static constexpr size_t MaxNumInputs = 4;
		explicit NodeArgument(Size2d size,
		                      double resolution,
		                      std::array<InputPortValue, MaxNumInputs> const& inputs)
		    : m_size{size}
		    , m_resolution{resolution}
		    , r_inputs{inputs}
		{
		}

		auto const& inputs() const { return r_inputs; }

		Size2d size() const { return m_size; }

		double resolution() const { return m_resolution; }

	private:
		Size2d m_size;
		double m_resolution;
		std::array<InputPortValue, MaxNumInputs> r_inputs;
	};
}

#endif