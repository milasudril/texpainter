//@	{
//@	 "targets":[{"name":"layer.o", "type":"object"}]
//@	}

#include "./layer.hpp"

void Texpainter::Model::render(Layer const& layer, Span2d<Pixel> ret)
{
	auto const ϴ            = layer.rotation();
	auto const rot_x        = vec2_t{cos(ϴ), sin(ϴ)};
	auto const rot_y        = vec2_t{-sin(ϴ), cos(ϴ)};
	auto const scale_factor = 1.0 / layer.scaleFactor();

	auto const src = layer.content().pixels();
	auto const origin_src =
	    0.5 * vec2_t{static_cast<double>(src.width()), static_cast<double>(src.height())};
	auto const loc_src_ret_coord =
	    layer.location()
	    + 0.5 * vec2_t{static_cast<double>(ret.width()), static_cast<double>(ret.height())};

	auto const aabb         = 0.5 * axisAlignedBoundingBox(layer);
	auto const begin_coords = loc_src_ret_coord - aabb;
	auto const end_coords   = loc_src_ret_coord + aabb;

	for(int row = static_cast<int>(begin_coords[1]); row < static_cast<int>(end_coords[1]); ++row)
	{
		for(int col = static_cast<int>(begin_coords[0]); col < static_cast<int>(end_coords[0]);
		    ++col)
		{
			auto const loc_ret = vec2_t{static_cast<double>(col), static_cast<double>(row)};
			auto const src_pos =
			    scale_factor * transform(loc_ret - loc_src_ret_coord, rot_x, rot_y) + origin_src;
			if(src_pos[0] >= 0 && src_pos[0] < src.width() && src_pos[1] >= 0
			   && src_pos[1] < src.height())
			{
				auto const src_pixel =
				    src(static_cast<uint32_t>(src_pos[0]), static_cast<uint32_t>(src_pos[1]));
				auto& ret_pixel = ret(col % ret.width(), row % ret.height());
				ret_pixel = src_pixel.alpha() * src_pixel + (1.0f - src_pixel.alpha()) * ret_pixel;
			}
		}
	}
}

Texpainter::Model::Layer& Texpainter::Model::Layer::paint(vec2_t origin, double radius, Pixel color)
{
	auto const w = m_content->width();
	auto const h = m_content->height();

	origin /= m_scale;
	auto const offset = 0.5 * vec2_t{static_cast<double>(w), static_cast<double>(h)};
	origin += offset - m_loc / m_scale;
	auto const begin_coords = origin - vec2_t{radius, radius};
	auto const end_coords   = origin + vec2_t{radius, radius};
	auto pixels             = m_content->pixels();

	for(int row = static_cast<int>(begin_coords[1]); row <= static_cast<int>(end_coords[1]); ++row)
	{
		for(int col = static_cast<int>(begin_coords[0]); col <= static_cast<int>(end_coords[0]);
		    ++col)
		{
			auto const loc_ret = vec2_t{static_cast<double>(col), static_cast<double>(row)};
			auto d             = loc_ret - origin;
			if(dot(d, d) < radius * radius) { pixels(col % w, row % h) = color; }
		}
	}
	return *this;
}