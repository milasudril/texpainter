//@	{
//@	 "targets":[{"name":"paint.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"paint.o","rel":"implementation"}]
//@	}

#include "./brush.hpp"

#include "pixel_store/pixel.hpp"
#include "utils/vec_t.hpp"
#include "utils/span_2d.hpp"

namespace Texpainter::Model
{
	void paint(Span2d<PixelStore::Pixel> pixels,
	           vec2_t origin,
	           double radius,
	           BrushFunction brush,
	           PixelStore::Pixel color);

	void floodfill(Span2d<PixelStore::Pixel> pixels, vec2_t origin, PixelStore::Pixel color);

	void drawOutline(Span2d<PixelStore::Pixel> pixels);
}