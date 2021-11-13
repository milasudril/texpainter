//@	{
//@	 "targets":[{"name":"minmax_filter.test","type":"application", "autorun":1}]
//@	}

#include "./minmax_filter.hpp"

#include "pixel_store/image.hpp"

namespace Testcases
{
	void texpainterRollingRankFilterMinmaxFilter()
	{
		Texpainter::PixelStore::Image<float> input{8, 6};
		std::generate_n(input.pixels().data(), area(input), [counter = 0]() mutable {
			++counter;
			return static_cast<float>(counter);
		});

		assert(area(input) == 48);

		Texpainter::PixelStore::Image<int8_t> mask{6, 5};
		std::ranges::fill(mask.pixels(), 0);
		for(uint32_t y = 0; y != mask.height() - 1; ++y)
		{
			for(uint32_t x = 0; x != mask.width() - 1; ++x)
			{
				if(y % 2 == 0) { mask(x, y) = x % 2; }
				else
				{
					mask(x, y) = (x + 1) % 2;
				}
			}
		}

		assert(area(mask) == 30);

		Texpainter::PixelStore::Image<float> min{8, 6};
		Texpainter::PixelStore::Image<float> max{8, 6};

		Texpainter::RollingRankFilter::minmaxFilter(input, mask, min, max);
	}
}

int main()
{
	Testcases::texpainterRollingRankFilterMinmaxFilter();
	return 0;
}

#if 0

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
			auto const sample_x = (x + src.width() - mask.width() / 2) % src.width();
			auto const sample_y = (y + src.height() - mask.height() / 2) % src.height();
			sorted_vals.insert(src(sample_x, sample_y));
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
#endif