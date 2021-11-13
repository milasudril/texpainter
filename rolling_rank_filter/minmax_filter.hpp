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

	Delta genXDelta(Span2d<int8_t const> src);
}

#endif