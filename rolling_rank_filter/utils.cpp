//@	{
//@	 "targets":[{"name":"utils.o","type":"object"}]
//@	}

#include "./utils.hpp"

#include <algorithm>
#include <random>

Texpainter::RollingRankFilter::Delta Texpainter::RollingRankFilter::genXDelta(
    Span2d<int8_t const> src)
{
	Delta ret;
	ret.to_erase.reserve(src.width() + src.height());
	ret.to_insert.reserve(src.width() + src.height());
	for(uint32_t y = 0; y != src.height(); ++y)
	{
		if(auto const delta = -src(0, y); delta != 0)
		{ ret.to_erase.push_back(Location{static_cast<uint16_t>(0), static_cast<uint16_t>(y)}); }

		for(uint32_t x = 1; x != src.width(); ++x)
		{
			auto const delta = src(x - 1, y) - src(x, y);
			if(delta < 0)
			{
				ret.to_erase.push_back(
				    Location{static_cast<uint16_t>(x), static_cast<uint16_t>(y)});
			}
			if(delta > 0)
			{
				ret.to_insert.push_back(
				    Location{static_cast<uint16_t>(x), static_cast<uint16_t>(y)});
			}
		}
	}
	return ret;
}

Texpainter::RollingRankFilter::Delta Texpainter::RollingRankFilter::genXYDelta(
    Span2d<int8_t const> src)
{
	Delta ret;
	ret.to_erase.reserve(src.width() + src.height());
	ret.to_insert.reserve(src.width() + src.height());

	for(uint32_t x = 0; x != src.width(); ++x)
	{
		if(auto const delta = -src(x, 0); delta != 0)
		{ ret.to_erase.push_back(Location{static_cast<uint16_t>(x), static_cast<uint16_t>(0)}); }
	}

	for(uint32_t y = 1; y != src.height(); ++y)
	{
		if(auto const delta = -src(0, y - 1); delta != 0)
		{
			ret.to_erase.push_back(
			    Location{static_cast<uint16_t>(0), static_cast<uint16_t>(y - 1)});
		}

		for(uint32_t x = 1; x != src.width(); ++x)
		{
			auto const delta = src(x - 1, y - 1) - src(x, y);
			if(delta < 0)
			{
				ret.to_erase.push_back(
				    Location{static_cast<uint16_t>(x), static_cast<uint16_t>(y)});
			}
			if(delta > 0)
			{
				ret.to_insert.push_back(
				    Location{static_cast<uint16_t>(x), static_cast<uint16_t>(y)});
			}
		}
	}
	return ret;
}

Texpainter::PixelStore::Image<int8_t> Texpainter::RollingRankFilter::quantize(
    Span2d<float const> src, float threshold)
{
	PixelStore::Image<int8_t> ret{src.width(), src.height()};
	std::ranges::transform(src, std::data(ret.pixels()), [threshold](auto val) -> int8_t {
		return val >= threshold ? 1 : 0;
	});
	return ret;
}

Texpainter::PixelStore::Image<int8_t> Texpainter::RollingRankFilter::quantize(
    Span2d<float const> src, DefaultRng::SeedValue seed_val)
{
	PixelStore::Image<int8_t> ret{src.width(), src.height()};
	std::ranges::transform(src,
	                       std::data(ret.pixels()),
	                       [rng = DefaultRng::Engine{seed_val}](auto val) mutable -> int8_t {
		                       auto const res = std::bernoulli_distribution{val}(rng);
		                       return res ? 1 : 0;
	                       });
	return ret;
}
