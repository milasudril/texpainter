//@	{
//@	 "targets":[{"name":"minmax_filter.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"minmax_filter.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_ROLLINGRANKFILTER_MINMAXFILTER_HPP
#define TEXPAINTER_ROLLINGRANKFILTER_MINMAXFILTER_HPP

#include "./utils.hpp"

#include "utils/span_2d.hpp"

#include <cstdint>

namespace Texpainter::RollingRankFilter
{
	void minmaxFilter(Span2d<float const> src, Span2d<int8_t const> mask, float* min, float* max);

	inline void minmaxFilter(Span2d<float const> src, float const* mask, float* min, float* max)
	{
		auto const binary_mask = quantize(Span2d{mask, src.width(), src.height()});
		auto const bb = boundingBox(binary_mask.pixels(), [](auto val) { return val > 0; });
		if(!bb.valid()) [[unlikely]] { return; }
		auto const mask_cropped = PixelStore::crop(binary_mask.pixels(), bb, 1, 1);
		minmaxFilter(src, mask_cropped.pixels(), min, max);
	}

	inline void minmaxFilter(Span2d<float const> src,
	                         float const* mask,
	                         float* min,
	                         float* max,
	                         DefaultRng::SeedValue seed_val)
	{
		auto const binary_mask = quantize(Span2d{mask, src.width(), src.height()}, seed_val);
		auto const bb = boundingBox(binary_mask.pixels(), [](auto val) { return val > 0; });
		if(!bb.valid()) [[unlikely]] { return; }
		auto const mask_cropped = PixelStore::crop(binary_mask.pixels(), bb, 1, 1);
		minmaxFilter(src, mask_cropped.pixels(), min, max);
	}
}

#endif