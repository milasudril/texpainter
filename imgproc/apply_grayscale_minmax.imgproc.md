# Apply grayscale minmax

This image processor computes a linear combination of a running min and max filter, given the domain specified by Mask. Pixel values in the mask > 0.5 are included, others are rejected.

## Input ports

__Source:__ (Grayscale image) The source image

__Mask:__ (Grayscale image) The mask to be used for the image

## Parameters

__Minmax:__ (= 0.5) Selects min vs max. 0.0 means min. 1.0 means max. 0.5 will be an approximation of the median.

## Output ports

__Output:__ (Grayscale image) The filtered image

## Implementation

The basic idea behind this implementation is to store pixel values under the mask in a multiset. For each pixel, the leading values are added to the multiset, while the outgoing values are removed. The first pixel will query all pixel values below the mask, following periodic boundary conditions.

To save heap allocations, the multiset is preallocated. In order to compute the maximum size needed for this multiset, the number of white pixels (using the 0.5 threshold) in the mask are countet. To save iterations when probing the mask, its bounding box is used.

__Includes:__

```c++
#include "utils/preallocated_multiset.hpp"
#include "utils/span_2d.hpp"

#include <algorithm>
```

__Source code:__

```c++
auto createMultiset(std::span<float const> mask)
{
	auto const n = std::ranges::count_if(mask, [](auto val) { return val >= 0.5f; });
	return Texpainter::PreallocatedMultiset<float>{static_cast<size_t>(n)};
}

struct BoundingBox
{
	uint32_t y_min;
	uint32_t y_max;
	uint32_t x_min;
	uint32_t x_max;
};

auto computeYmin(Texpainter::Span2d<float const> mask)
{
	for(uint32_t y = 0; y != mask.height(); ++y)
	{
		for(uint32_t x = 0; x != mask.width(); ++x)
		{
			if(mask(x, y) >= 0.5f) { return y; }
		}
	}
	return mask.height();
}

auto computeYmax(Texpainter::Span2d<float const> mask)
{
	for(uint32_t y = mask.height(); y != 0; --y)
	{
		for(uint32_t x = 0; x != mask.width(); ++x)
		{
			if(mask(x, y - 1) >= 0.5f) { return y - 1; }
		}
	}
	return 0u;
}

auto computeXmin(Texpainter::Span2d<float const> mask)
{
	for(uint32_t x = 0; x != mask.width(); ++x)
	{
		for(uint32_t y = 0; y != mask.height(); ++y)
		{
			if(mask(x, y) >= 0.5f) { return x; }
		}
	}
	return mask.width();
}

auto computeXmax(Texpainter::Span2d<float const> mask)
{
	for(uint32_t x = mask.width(); x != 0; --x)
	{
		for(uint32_t y = 0; y != mask.height(); ++y)
		{
			if(mask(x - 1, y) >= 0.5f) { return x - 1; }
		}
	}
	return 0u;
}

auto computeBoundingBox(Texpainter::Span2d<float const> mask)
{
	return BoundingBox{computeYmin(mask), computeYmax(mask), computeXmin(mask), computeXmax(mask)};
}

void main(auto const& args, auto const&)
{
	auto const size = args.canvasSize();
	auto const pixel_count = area(args.canvasSize());
	auto sorted_vals       = createMultiset(std::span{input<1>(args), pixel_count});
	auto bb = computeBoundingBox(Texpainter::Span2d{input<1>(args), size});

	printf("(%u, %u) (%u, %u)\n", bb.x_min, bb.y_min, bb.x_max, bb.y_max);
}
```

## Tags

__Id:__ f5898fc46c3d7f70c7c8489a40b84b0d

__Category:__ Filters

__Release state:__ Stable