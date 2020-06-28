//@	{
//@	 "targets":[{"name":"layer.o", "type":"object"}]
//@	}

#include "./layer.hpp"

void Texpainter::Model::render(Layer const& layer, Span2d<Pixel> ret)
{
	//	auto aabb = axisAlignedBoundingBox(layer);
	//	Image ret{static_cast<uint32_t>(aabb[0]), static_cast<uint32_t>(aabb[1])};
	//	std::ranges::fill(ret, Pixel{0.0, 0.0, 0.0, 0.0,});
	//	auto const rot_x = vec2_t{cos(ϴ), sin(ϴ)};
	//	auto const rot_y = vec2_t{-sin(ϴ), cos(ϴ)};

	auto const src = layer.content().pixels();
	//	auto const origin_ret = vec2_t{ret.width()/2.0, ret.height()/2.0};
	auto const origin_src = vec2_t{src.width() / 2.0, src.height() / 2.0};
	auto const loc_src_ret_coord = layer.location();
	for(uint32_t row = 0; row < ret.height(); ++row)
	{
		for(uint32_t col = 0; col < ret.width(); ++col)
		{
			auto const loc_ret = vec2_t{static_cast<double>(col), static_cast<double>(row)};
			auto const src_pos = loc_ret - loc_src_ret_coord + origin_src;
			if(src_pos[0] >= 0 && src_pos[0] < src.width() && src_pos[1] >= 0 && src_pos[1] < src.height())
			{ ret(col, row) = src(static_cast<uint32_t>(src_pos[0]), static_cast<uint32_t>(src_pos[1])); }

			/*			auto src_pos = transform(vec2_t{static_cast<double>(col), static_cast<double>(row)} - mid,
			   rot_x, rot_y)
			   + */
		}
	}
}
