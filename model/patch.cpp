//@	{
//@	 "targets":[{"name":"patch.o", "type":"object"}]
//@	}

#include "./patch.hpp"

namespace
{
	void draw_marker(Texpainter::vec2_t loc,
	                 Texpainter::Span2d<Texpainter::PixelStore::Pixel> canvas)
	{
		constexpr auto radius   = 2;
		auto const w            = canvas.width();
		auto const h            = canvas.height();
		auto const begin_coords = loc - Texpainter::vec2_t{radius, radius};
		auto const end_coords   = loc + Texpainter::vec2_t{radius, radius};

		for(int row = static_cast<int>(begin_coords[1]); row <= static_cast<int>(end_coords[1]);
		    ++row)
		{
			for(int col = static_cast<int>(begin_coords[0]); col <= static_cast<int>(end_coords[0]);
			    ++col)
			{
				auto const loc_ret =
				    Texpainter::vec2_t{static_cast<double>(col), static_cast<double>(row)};
				auto d = loc_ret - loc;
				if(Texpainter::dot(d, d) < radius * radius)
				{
					auto& pixel = canvas((col + w) % w, (row + h) % h);
					pixel       = Texpainter::PixelStore::Pixel{1.0f, 1.0f, 1.0f, 1.0f} - pixel;
					pixel.alpha(1.0f);
				}
			}
		}
	}
}

Texpainter::Model::Patch& Texpainter::Model::Patch::paint(vec2_t origin,
                                                          float radius,
                                                          BrushFunction brush,
                                                          PixelStore::Pixel color)
{
	auto const w = m_image.width();
	auto const h = m_image.height();

	auto const offset = 0.5 * vec2_t{static_cast<double>(w), static_cast<double>(h)};
	auto const ϴ      = m_rot;
	auto const rot_x  = vec2_t{cos(ϴ), sin(ϴ)};
	auto const rot_y  = vec2_t{-sin(ϴ), cos(ϴ)};
	origin            = transform(origin - m_loc, rot_x, rot_y) / m_scale + offset;

	auto const r_vec        = vec2_t{static_cast<double>(radius), static_cast<double>(radius)};
	auto const begin_coords = origin - r_vec;
	auto const end_coords   = origin + r_vec;
	auto pixels             = m_image.pixels();

	for(int row = static_cast<int>(begin_coords[1]); row <= static_cast<int>(end_coords[1]); ++row)
	{
		for(int col = static_cast<int>(begin_coords[0]); col <= static_cast<int>(end_coords[0]);
		    ++col)
		{
			auto const loc_ret = vec2_t{static_cast<double>(col), static_cast<double>(row)};
			auto d             = loc_ret - origin;
			if(brush(radius, d)) { pixels((col + w) % w, (row + h) % h) = color; }
		}
	}
	return *this;
}

void Texpainter::Model::outline(Patch const& patch, Span2d<PixelStore::Pixel> canvas)
{
	auto const w      = canvas.width();
	auto const h      = canvas.height();
	auto const offset = 0.5 * vec2_t{static_cast<double>(w), static_cast<double>(h)};

	auto const origin       = patch.location() + offset;
	auto const ϴ            = patch.rotation();
	auto const rot_x        = vec2_t{cos(ϴ), -sin(ϴ)};
	auto const rot_y        = vec2_t{sin(ϴ), cos(ϴ)};
	auto const scale_factor = patch.scaleFactor();

	auto const size_patch = patch.size();
	auto const box_patch =
	    0.5 * scale_factor
	    * vec2_t{static_cast<double>(size_patch.width()), static_cast<double>(size_patch.height())};
	draw_marker(origin, canvas);
	draw_marker(origin + transform(box_patch * vec2_t{1.0, 1.0}, rot_x, rot_y), canvas);
	draw_marker(origin + transform(box_patch * vec2_t{-1.0, 1.0}, rot_x, rot_y), canvas);
	draw_marker(origin + transform(box_patch * vec2_t{-1.0, -1.0}, rot_x, rot_y), canvas);
	draw_marker(origin + transform(box_patch * vec2_t{1.0, -1.0}, rot_x, rot_y), canvas);
}

void Texpainter::Model::render(Patch const& patch, Span2d<PixelStore::Pixel> ret, double scale)
{
	auto const ϴ            = patch.rotation();
	auto const rot_x        = vec2_t{cos(ϴ), sin(ϴ)};
	auto const rot_y        = vec2_t{-sin(ϴ), cos(ϴ)};
	auto const scale_factor = 1.0 / (scale * patch.scaleFactor());

	auto const& src = patch.image();
	auto const origin_src =
	    0.5 * vec2_t{static_cast<double>(src.width()), static_cast<double>(src.height())};
	auto const loc_src_ret_coord =
	    patch.location() * scale
	    + 0.5 * vec2_t{static_cast<double>(ret.width()), static_cast<double>(ret.height())};

	auto const aabb         = 0.5 * axisAlignedBoundingBox(patch, scale);
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
				auto& ret_pixel =
				    ret((col + ret.width()) % ret.width(), (row + ret.height()) % ret.height());
				ret_pixel = src_pixel;
			}
		}
	}
}