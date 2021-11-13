//@	{
//@	 "targets":[{"name":"minmax_filter.o","type":"object"}]
//@	}

#include "./minmax_filter.hpp"

#include <vector>

std::vector<Texpainter::RollingRankFilter::Delta> Texpainter::RollingRankFilter::genLineDelta(
    Span2d<int8_t const> src)
{
	std::vector<Delta> ret;
	ret.reserve(2 * (src.width() + src.height()));
	for(uint32_t y = 0; y != src.height(); ++y)
	{
		if(auto const delta = -src(0, y); delta != 0)
		{
			ret.push_back(
			    Delta{static_cast<uint16_t>(0), static_cast<uint16_t>(y), delta < 0 ? -1 : 1});
		}

		for(uint32_t x = 1; x != src.width(); ++x)
		{
			if(auto const delta = src(x - 1, y) - src(x, y); delta != 0)
			{
				ret.push_back(
				    Delta{static_cast<uint16_t>(x), static_cast<uint16_t>(y), delta < 0 ? -1 : 1});
			}
		}
	}
	return ret;
}