//@	{
//@	 "targets":[{"name":"minmax_filter.o","type":"object"}]
//@	}

#include "./minmax_filter.hpp"
#include "./utils.hpp"

#include "utils/preallocated_multiset.hpp"

void Texpainter::RollingRankFilter::minmaxFilter(Span2d<float const> src,
                                                 Span2d<int8_t const> mask,
                                                 Span2d<float> min,
                                                 Span2d<float> max)
{
	auto const x_delta  = genXDelta(mask);
	auto const xy_delta = genXYDelta(mask);
	auto const mask_size =
	    static_cast<size_t>(std::ranges::count_if(mask, [](auto val) { return val == 1; }));
	PreallocatedMultiset<float> sorted_vals{mask_size};

	for(uint32_t y = 0; y != mask.height(); ++y)
	{
		for(uint32_t x = 0; x != mask.width(); ++x)
		{
			if(mask(x, y) != 0)
			{
				auto const sample_x = (x + src.width() - mask.width() / 2) % src.width();
				auto const sample_y = (y + src.height() - mask.height() / 2) % src.height();
				sorted_vals.insert(src(sample_x, sample_y));
			}
		}
	}

	for(uint32_t y = 0; y != src.height(); ++y)
	{
		for(uint32_t x = 0; x != src.width(); ++x)
		{
			min(x, y) = sorted_vals.front();
			max(x, y) = sorted_vals.back();
		}
	}
}