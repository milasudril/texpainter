//@	{
//@	 "targets":[{"name":"minmax_filter.o","type":"object"}]
//@	}

#include "./minmax_filter.hpp"
#include "./utils.hpp"

#include "utils/preallocated_multiset.hpp"

namespace
{
	void update(Texpainter::Span2d<float const> src,
	            uint32_t x,
	            uint32_t y,
	            Texpainter::PreallocatedMultiset<float>& sorted_vals,
	            uint32_t mask_width,
	            uint32_t mask_height,
	            Texpainter::RollingRankFilter::Delta const& delta)
	{
		auto const sample_x = x + src.width() - mask_width / 2;
		auto const sample_y = y + src.height() - mask_height / 2;
		std::ranges::for_each(delta.to_erase, [&src, sample_x, sample_y, &sorted_vals](auto val) {
			sorted_vals.erase_one(
			    src((sample_x + val.x) % src.width(), (sample_y + val.y) % src.height()));
		});
		std::ranges::for_each(delta.to_insert, [&src, sample_x, sample_y, &sorted_vals](auto val) {
			sorted_vals.insert(
			    src((sample_x + val.x) % src.width(), (sample_y + val.y) % src.height()));
		});
	}
}

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
		for(uint32_t x = 0; x != src.width() - 1; ++x)
		{
			min(x, y) = sorted_vals.front();
			max(x, y) = sorted_vals.back();

			update(src, x, y, sorted_vals, mask.width(), mask.height(), x_delta);
		}

		auto const x = src.width() - 1;

		min(x, y) = sorted_vals.front();
		max(x, y) = sorted_vals.back();

		update(src, x, y, sorted_vals, mask.width(), mask.height(), xy_delta);
	}
}