//@	{
//@	  "targets":[{"name":"compositor_output.hpp", "type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"compositor_output.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_MODEL_COMPOSITOROUTPUT_HPP
#define TEXPAINTER_MODEL_COMPOSITOROUTPUT_HPP

#include "filtergraph/port_value.hpp"
#include "utils/size_2d.hpp"

namespace Texpainter::Model
{
	class CompositorOutput
	{
	public:
		explicit CompositorOutput(Size2d size,
		                          FilterGraph::PortValue const& src,
		                          uint32_t resolution);

		Size2d size() const { return m_size; }

		FilterGraph::PortValue const& data() const { return m_data; }

	private:
		Size2d m_size;
		FilterGraph::PortValue m_data;
	};
}

#endif