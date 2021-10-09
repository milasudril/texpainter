//@	{
//@	  "targets":[{"name":"compositor_output.o", "type":"object"}]
//@	}

#include "./compositor_output.hpp"

namespace
{
	template<class T>
	auto downsample(Texpainter::Size2d, T const&, uint32_t)
	{
		return T{};
	}

	Texpainter::FilterGraph::PortValue downsample_impl(
	    Texpainter::Size2d size, Texpainter::FilterGraph::PortValue const& src, uint32_t resolution)
	{
		return visit(
		    [size, resolution](auto const& item) {
			    return Texpainter::FilterGraph::PortValue{downsample(size, item, resolution)};
		    },
		    src);
	}
}

Texpainter::Model::CompositorOutput::CompositorOutput(Size2d size,
                                                      FilterGraph::PortValue const& src,
                                                      uint32_t resolution)
    : m_size{size.width() / resolution, size.height() / resolution}
    , m_data{downsample_impl(size, src, resolution)}
{
}
