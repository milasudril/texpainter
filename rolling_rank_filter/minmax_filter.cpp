//@	{
//@	 "targets":[{"name":"minmax_filter.o","type":"object", "dependencies":[{"ref":"pthread","rel":"external"}]}]
//@	}

#include "./minmax_filter.hpp"
#include "./utils.hpp"

#include "utils/preallocated_multiset.hpp"

#include <cmath>
#include <thread>

namespace
{
	struct FloatCompare
	{
		bool operator()(float a, float b) const
		{
			if(std::isnan(a)) [[unlikely]]
			{
				return !std::isnan(b);
			}
			return a < b;
		}
	};

	void update(Texpainter::Span2d<float const> src,
	            uint32_t x,
	            uint32_t y,
	            Texpainter::PreallocatedMultiset<float, FloatCompare>& sorted_vals,
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

void Texpainter::RollingRankFilter::minmaxFilter(
    Span2d<float const> src, Span2d<int8_t const> mask, float* min, float* max, ScanlineRange range)
{
	auto const x_delta  = genXDelta(mask);
	auto const xy_delta = genXYDelta(mask);
	auto const mask_size =
	    static_cast<size_t>(std::ranges::count_if(mask, [](auto val) { return val == 1; }));

	PreallocatedMultiset<float, FloatCompare> sorted_vals{mask_size};
	for(uint32_t y = 0; y != mask.height(); ++y)
	{
		for(uint32_t x = 0; x != mask.width(); ++x)
		{
			if(mask(x, y) != 0)
			{
				auto const sample_x = (x + src.width() - mask.width() / 2) % src.width();
				auto const sample_y =
				    (y + range.first + src.height() - mask.height() / 2) % src.height();
				sorted_vals.insert(src(sample_x, sample_y));
			}
		}
	}

	for(uint32_t y = range.first; y != range.last; ++y)
	{
		for(uint32_t x = 0; x != src.width() - 1; ++x)
		{
			min[y * src.width() + x] = sorted_vals.front();
			max[y * src.width() + x] = sorted_vals.back();

			update(src, x, y, sorted_vals, mask.width(), mask.height(), x_delta);
		}

		auto const x = src.width() - 1;

		min[y * src.width() + x] = sorted_vals.front();
		max[y * src.width() + x] = sorted_vals.back();

		update(src, x, y, sorted_vals, mask.width(), mask.height(), xy_delta);
	}
}

void Texpainter::RollingRankFilter::minmaxFilter(Span2d<float const> src,
                                                 Span2d<int8_t const> mask,
                                                 float* min,
                                                 float* max)
{
	auto const num_workers = std::max(1u, std::thread::hardware_concurrency());
	std::vector<std::jthread> workers;
	workers.reserve(num_workers);

	auto const lines_per_thread = std::max(1u, src.height() / num_workers);
	auto num_lines_left         = src.height();
	uint32_t line_start         = 0;
	while(num_lines_left != 0)
	{
		auto const batch_size = std::min(num_lines_left, lines_per_thread);
		workers.push_back(
		    std::jthread([lines_per_thread,
		                  src,
		                  mask,
		                  min,
		                  max,
		                  range = ScanlineRange{line_start, line_start + batch_size}]() {
			    minmaxFilter(src, mask, min, max, range);
		    }));
		line_start += batch_size;
		num_lines_left -= batch_size;
	}
}