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

		Texpainter::RollingRankFilter::minmaxFilter(
		    input, mask, min.pixels().data(), max.pixels().data());
	}
}

int main()
{
	Testcases::texpainterRollingRankFilterMinmaxFilter();
	return 0;
}