//@	{
//@	 "targets":[{"name":"minmax_filter.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"minmax_filter.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_ROLLINGRANKFILTER_MINMAXFILTER_HPP
#define TEXPAINTER_ROLLINGRANKFILTER_MINMAXFILTER_HPP

#include "pixel_store/image.hpp"

#include <vector>
#include <cstdint>

namespace Texpainter::RollingRankFilter
{
	struct Location
	{
		uint16_t x;
		uint16_t y;
	};

	struct Delta
	{
		std::vector<Location> to_erase;
		std::vector<Location> to_insert;
	};

	PixelStore::Image<int8_t> quantize(Span2d<float const> src, float threshold = 0.5f);

	Delta genXDelta(Span2d<int8_t const> src);

	Delta genXYDelta(Span2d<int8_t const> src);


}

#endif