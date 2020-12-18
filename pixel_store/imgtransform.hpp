//@	{
//@	 "targets":[{"name":"imgtransform.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_PIXELSTORE_IMGTRANSFORM_HPP
#define TEXPAINTER_PIXELSTORE_IMGTRANSFORM_HPP

#include "utils/span_2d.hpp"
#include "utils/vec_t.hpp"

namespace Texpainter::PixelStore
{
	template<class PixelType>
	void renderTiled(Span2d<PixelType const> src, PixelType* sink, vec2_t s)
	{
		auto const w = src.size().width();
		auto const h = src.size().height();

		auto const O = 0.5 * vec2_t{static_cast<double>(w), static_cast<double>(h)};

		for(uint32_t row = 0; row < h; ++row)
		{
			for(uint32_t col = 0; col < w; ++col)
			{
				auto const loc       = vec2_t{static_cast<double>(col), static_cast<double>(row)};
				auto const src_pixel = O + (loc - O) / s;
				auto const src_x     = static_cast<int32_t>(src_pixel[0]);
				auto const src_y     = static_cast<int32_t>(src_pixel[1]);
				sink[row * w + col]  = src((w + src_x) % w, (h + src_y) % h);
			}
		}
	}

	template<class PixelType>
	void renderCentered(Span2d<PixelType const> src, PixelType* sink, vec2_t s)
	{
		auto const w = src.size().width();
		auto const h = src.size().height();

		auto const O = 0.5 * vec2_t{static_cast<double>(w), static_cast<double>(h)};

		auto const size_vec     = vec2_t{static_cast<double>(w), static_cast<double>(h)};
		auto const bounding_box = 0.5 * Texpainter::axisAlignedBoundingBox(s * size_vec);
		auto const begin_coords = O - bounding_box;
		auto const end_coords   = O + bounding_box;

		for(int row = static_cast<int>(begin_coords[1]); row < static_cast<int>(end_coords[1]);
		    ++row)
		{
			for(int col = static_cast<int>(begin_coords[0]); col < static_cast<int>(end_coords[0]);
			    ++col)
			{
				auto const loc       = vec2_t{static_cast<double>(col), static_cast<double>(row)};
				auto const src_pixel = O + (loc - O) / s;
				auto const src_x =
				    std::clamp(static_cast<int32_t>(src_pixel[0]), 0, static_cast<int32_t>(w - 1));
				auto const src_y =
				    std::clamp(static_cast<int32_t>(src_pixel[1]), 0, static_cast<int32_t>(h - 1));
				sink[row * w + col] = src(src_x, src_y);
			}
		}
	}

	template<class PixelType>
	void renderTiled(Span2d<PixelType const> src, PixelType* sink, Angle ϴ)
	{
		auto const rot_x    = vec2_t{cos(ϴ), -sin(ϴ)};
		auto const rot_y    = vec2_t{sin(ϴ), cos(ϴ)};
		auto const w        = src.size().width();
		auto const h        = src.size().height();
		auto const size_vec = vec2_t{static_cast<double>(w), static_cast<double>(h)};

		auto const O = 0.5 * size_vec;

		for(uint32_t row = 0; row < h; ++row)
		{
			for(uint32_t col = 0; col < w; ++col)
			{
				auto const loc_ret = vec2_t{static_cast<double>(col), static_cast<double>(row)};
				auto const src_pos = Texpainter::transform(loc_ret - O, rot_x, rot_y) + O;
				auto src_x         = static_cast<int32_t>(src_pos[0]);
				auto src_y         = static_cast<int32_t>(src_pos[1]);

				sink[row * w + col] = src((src_x + w) % w, (src_y + h) % h);
			}
		}
	}

	template<class PixelType>
	void renderCentered(Span2d<PixelType const> src, PixelType* sink, Angle ϴ)
	{
		auto const rot_x    = vec2_t{cos(ϴ), -sin(ϴ)};
		auto const rot_y    = vec2_t{sin(ϴ), cos(ϴ)};
		auto const w        = src.size().width();
		auto const h        = src.size().height();
		auto const size_vec = vec2_t{static_cast<double>(w), static_cast<double>(h)};

		auto const O = 0.5 * size_vec;

		for(uint32_t row = 0; row < h; ++row)
		{
			for(uint32_t col = 0; col < w; ++col)
			{
				auto const loc_ret = vec2_t{static_cast<double>(col), static_cast<double>(row)};
				auto const src_pos = Texpainter::transform(loc_ret - O, rot_x, rot_y) + O;
				auto src_x         = static_cast<int32_t>(src_pos[0]);
				auto src_y         = static_cast<int32_t>(src_pos[1]);

				if(src_pos[0] >= 0 && src_pos[0] < w && src_pos[1] >= 0 && src_pos[1] < h)
					[[likely]] { sink[row * w + col] = src(src_x, src_y); }
			}
		}
	}
}

#endif