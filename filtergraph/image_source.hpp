//@	{
//@	 "targets":[{"name":"image_source.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_IMAGESOURCE_HPP
#define TEXPAINTER_FILTERGRAPH_IMAGESOURCE_HPP

namespace Texpainter::FilterGraph
{
	template<class PixelType>
	class ImageSource
	{
	public:
	private:
		Span2d<PixelType const> r_pixels;
	};
}

#endif
