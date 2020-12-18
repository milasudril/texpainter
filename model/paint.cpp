//@	{"targets":[{"name":"paint.o","type":"object"}]}

#include "./paint.hpp"

void Texpainter::Model::paint(Span2d<PixelStore::Pixel> pixels,
                              vec2_t origin,
                              double radius,
                              BrushFunction brush,
                              PixelStore::Pixel color)
{
	auto const w = pixels.width();
	auto const h = pixels.height();

	auto const r_vec        = vec2_t{radius, radius};
	auto const begin_coords = origin - r_vec;
	auto const end_coords   = origin + r_vec;

	for(int row = static_cast<int>(begin_coords[1]); row <= static_cast<int>(end_coords[1]); ++row)
	{
		for(int col = static_cast<int>(begin_coords[0]); col <= static_cast<int>(end_coords[0]);
		    ++col)
		{
			auto const loc_ret = vec2_t{static_cast<double>(col), static_cast<double>(row)};
			auto d             = loc_ret - origin;
			if(brush(d, radius)) { pixels((col + w) % w, (row + h) % h) = color; }
		}
	}
}
