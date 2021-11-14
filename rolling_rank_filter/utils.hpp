//@	{
//@	 "targets":[{"name":"utils.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"utils.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_ROLLINGRANKFILTER_UTILS_HPP
#define TEXPAINTER_ROLLINGRANKFILTER_UTILS_HPP

#include "pixel_store/image.hpp"
#include "utils/default_rng.hpp"

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

	PixelStore::Image<int8_t> quantize(Span2d<float const> src, DefaultRng::SeedValue seed_val);

	Delta genXDelta(Span2d<int8_t const> src);

	Delta genXYDelta(Span2d<int8_t const> src);


}

#endif