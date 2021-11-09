# Apply grayscale minmax

This image processor computes a linear combination of a running min and max filter, given the domain specified by Mask.

## Input ports

__Source:__ (Grayscale image) The source image

__Mask:__ (Grayscale image) The mask to be used for the image

## Output ports

__Output:__ (Grayscale image) The filtered image

## Implementation

__Includes:__ 

```c++
#include "pixel_store/image.hpp"
#include "utils/span_2d.hpp"
#include "utils/preallocated_multiset.hpp"

#include <algorithm>
#include <chrono>
```

__Source code:__ 

```c++
struct BoundingBox
{
	uint32_t y_min;
	uint32_t y_max;
	uint32_t x_min;
	uint32_t x_max;
};

inline auto computeYmin(Texpainter::Span2d<float const> mask, float threshold)
{
	for(uint32_t y = 0; y != mask.height(); ++y)
	{
		for(uint32_t x = 0; x != mask.width(); ++x)
		{
			if(mask(x, y) >= threshold) { return y; }
		}
	}
	return mask.height();
}

inline auto computeYmax(Texpainter::Span2d<float const> mask, float threshold)
{
	for(uint32_t y = mask.height(); y != 0; --y)
	{
		for(uint32_t x = 0; x != mask.width(); ++x)
		{
			if(mask(x, y - 1) >= threshold) { return y; }
		}
	}
	return 0u;
}

inline auto computeXmin(Texpainter::Span2d<float const> mask, float threshold)
{
	for(uint32_t x = 0; x != mask.width(); ++x)
	{
		for(uint32_t y = 0; y != mask.height(); ++y)
		{
			if(mask(x, y) >= threshold) { return x; }
		}
	}
	return mask.width();
}

inline auto computeXmax(Texpainter::Span2d<float const> mask, float threshold)
{
	for(uint32_t x = mask.width(); x != 0; --x)
	{
		for(uint32_t y = 0; y != mask.height(); ++y)
		{
			if(mask(x - 1, y) >= threshold) { return x; }
		}
	}
	return 0u;
}

inline auto computeBoundingBox(Texpainter::Span2d<float const> mask, float threshold)
{
	return BoundingBox{computeYmin(mask, threshold),
	                   computeYmax(mask, threshold),
	                   computeXmin(mask, threshold),
	                   computeXmax(mask, threshold)};
}

using Mask = Texpainter::PixelStore::Image<int8_t>;

inline auto crop(Texpainter::Span2d<float const> img, float threshold)
{
	auto const bb     = computeBoundingBox(img, threshold);
	auto const width  = bb.x_max - bb.x_min;
	auto const height = bb.y_max - bb.y_min;
	Mask ret{width + 1, height + 1};
	std::ranges::fill(ret.pixels(), 0);
	size_t N = 0;
	for(auto y = bb.y_min; y != bb.y_max; ++y)
	{
		for(auto x = bb.x_min; x != bb.x_max; ++x)
		{
			N += static_cast<size_t>(img(x, y) >= threshold);
			ret(x - bb.x_min, y - bb.y_min) = img(x, y) >= threshold ? 1 : 0;
		}
	}

	return std::pair{std::move(ret), N};
}

struct Delta
{
	uint16_t x;
	uint16_t y;
	int32_t sign;
};

inline auto computeDeltaCol(Texpainter::Span2d<int8_t const> mask)
{
	std::vector<Delta> deltas;
	deltas.reserve(2 * (mask.height() + mask.width() + 1));
	auto sum = 0;
	for(auto y = 0u; y != mask.height() - 1; ++y)
	{
		if(auto val = -mask(0, y); val != 0)
		{
			deltas.push_back(Delta{0u, static_cast<uint16_t>(y), val});
			sum += val;
		}

		for(auto x = 1u; x != mask.width(); ++x)
		{
			if(auto val = mask(x - 1, y) - mask(x, y); val != 0)
			{
				deltas.push_back(Delta{static_cast<uint16_t>(x), static_cast<uint16_t>(y), val});
				sum += val;
			}
		}
	}
	assert(sum == 0);
	return deltas;
}

inline auto computeDeltaRow(Texpainter::Span2d<int8_t const> mask)
{
	std::vector<Delta> deltas;
	deltas.reserve(2 * (mask.height() + mask.width() + 2));
	auto sum = 0;
	for(auto x = 0u; x != mask.width(); ++x)
	{
		if(auto val = -mask(x, 0u))
		{
			deltas.push_back(Delta{static_cast<uint16_t>(x), 0u, val});
			printf("- ");
			--sum;
		}
		else
		{
			printf(". ");
		}
	}
	printf("\n");

	for(auto y = 1u; y != mask.height(); ++y)
	{
		if(auto val = -mask(0u, y); val != 0)
		{
			deltas.push_back(Delta{0u, static_cast<uint16_t>(y), val});
			printf("- ");
			--sum;
		}
		else
		{
			printf(". ");
		}


		for(auto x = 1u; x != mask.width(); ++x)
		{
			if(auto val = mask(x - 1, y - 1) - mask(x, y); val != 0)
			{
				deltas.push_back(Delta{static_cast<uint16_t>(x), static_cast<uint16_t>(y), val});
				printf("%c ", val < 0 ? '-' : '+');
				sum += val;
			}
			else
			{
				printf(". ");
			}
		}

		if(auto val = mask(mask.width() - 1, y); val != 0)
		{
			deltas.push_back(
			    Delta{static_cast<uint16_t>(mask.width()), static_cast<uint16_t>(y), val});
			printf("+ ");
			++sum;
		}
		else
		{
			printf(". ");
		}
		printf("\n");
	}

	for(auto x = 0u; x != mask.width(); ++x)
	{
		if(auto val = mask(x, mask.height() - 1))
		{
			deltas.push_back(
			    Delta{static_cast<uint16_t>(x), static_cast<uint16_t>(mask.height() - 1), val});
			printf("+ ");
			++sum;
		}
		else
		{
			printf(". ");
		}
	}
	printf("\n");
	assert(sum == 0);
	return deltas;
}

void main(auto const& args)
{
	auto const size           = args.canvasSize();
	auto const start_time     = std::chrono::steady_clock::now();
	auto const mask           = crop(Texpainter::Span2d{input<1>(args), size}, 0.5f);
	auto const mask_delta_col = computeDeltaCol(mask.first.pixels());
	auto const mask_delta_row = computeDeltaRow(mask.first.pixels());
	auto const w              = size.width();
	auto const h              = size.height();
	auto const w_m            = mask.first.width();
	auto const h_m            = mask.first.height();

	Texpainter::PreallocatedMultiset<float> sorted_vals{mask.second};
	printf("Init %zu\n", (std::chrono::steady_clock::now() - start_time).count());

	for(auto y = 0u; y != h; ++y)
	{
		sorted_vals.clear();
		/*	for(auto x = 0u; x != w; ++x)
		{
			sorted_vals.erase_one(input<0>(args, x, (y + h - h_m / 2 - 1) % h));
		}*/

		//	printf("Erased %u\n", y);
		for(auto x = 0u; x != w; ++x)
		{
			std::ranges::for_each(
			    mask_delta_col, [x, y, &args, w, h, w_m, h_m, &sorted_vals](auto item) {
				    auto const x_m = item.x;
				    auto const y_m = item.y;
				    if(item.sign < 0)
				    {
					    sorted_vals.erase_one(input<0>(
					        args, (x + w + x_m - w_m / 2) % w, (y + h + y_m - h_m / 2) % h));
				    }
				    else
				    {
					    sorted_vals.insert(input<0>(
					        args, (x + w + x_m - w_m / 2) % w, (y + h + y_m - h_m / 2) % h));
				    }
			    });
			//	printf("%u %u %zu\n", x, y, std::size(sorted_vals));
			output<0>(args, x, y) = *std::begin(sorted_vals);
		}
	}
	printf("Render %zu\n", (std::chrono::steady_clock::now() - start_time).count());
}
```

## Tags

__Id:__ f5898fc46c3d7f70c7c8489a40b84b0d

__Category:__ Filters

__Release state:__ Stable