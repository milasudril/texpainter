//@	{
//@	 "targets":[{"name":"minmax_filter.o","type":"object"}]
//@	}

#include "./minmax_filter.hpp"

#include <vector>

Texpainter::RollingRankFilter::Delta Texpainter::RollingRankFilter::genLineDelta(
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