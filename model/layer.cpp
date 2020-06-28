//@	{
//@	 "targets":[{"name":"layer.o", "type":"object"}]
//@	}

#include "./layer.hpp"

void Texpainter::Model::render(Layer const& layer, Span2d<Pixel> ret)
{
	auto const ϴ = layer.rotation();
	auto const rot_x = vec2_t{cos(ϴ), sin(ϴ)};
	auto const rot_y = vec2_t{-sin(ϴ), cos(ϴ)};
	auto const scale_factor = 1.0 / layer.scaleFactor();

	auto const src = layer.content().pixels();
	auto const origin_src = vec2_t{src.width() / 2.0, src.height() / 2.0};
	auto const loc_src_ret_coord = layer.location();

	auto const aabb = 0.5 * axisAlignedBoundingBox(layer);
	auto const begin_coords = loc_src_ret_coord - aabb;
	auto const end_coords = loc_src_ret_coord + aabb;

	for(int row = static_cast<int>(begin_coords[1]); row < static_cast<int>(end_coords[1]); ++row)
	{
		for(int col = static_cast<int>(begin_coords[0]); col < static_cast<int>(end_coords[0]); ++col)
		{
			auto const loc_ret = vec2_t{static_cast<double>(col), static_cast<double>(row)};
			auto const src_pos =
			   scale_factor * transform(loc_ret - loc_src_ret_coord, rot_x, rot_y) + origin_src;
			if(src_pos[0] >= 0 && src_pos[0] < src.width() && src_pos[1] >= 0 && src_pos[1] < src.height())
			{
				//			printf("%u %u\n", col, row);
				ret(col, row) = src(static_cast<uint32_t>(src_pos[0]), static_cast<uint32_t>(src_pos[1]));
			}
		}
	}
}
