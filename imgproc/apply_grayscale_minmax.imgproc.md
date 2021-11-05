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
	return Texpainter::PreallocatedMultiset<float>{2*static_cast<size_t>(n)};
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
			if(mask(x, y - 1) >= 0.5f) { return y; }
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
			if(mask(x - 1, y) >= 0.5f) { return x; }
		}
	}
	return 0u;
}

auto computeBoundingBox(Texpainter::Span2d<float const> mask)
{
	return BoundingBox{computeYmin(mask), computeYmax(mask), computeXmin(mask), computeXmax(mask)};
}

struct MaskOrigin
{
	int32_t x;
	int32_t y;
};

void eraseCol(Texpainter::PreallocatedMultiset<float>& set,
	MaskOrigin origin,
	Texpainter::Span2d<float const> source,
	float const*,
	BoundingBox bb,
	uint32_t mask_x)
{
	for(uint32_t mask_y = bb.y_min; mask_y != bb.y_max; ++mask_y)
	{
		auto const src_x = (origin.x + mask_x + source.width())%source.width();
		auto const src_y = (origin.y + mask_y + source.height())%source.height();
	//	auto const mask_val = mask[mask_y * source.width() + mask_x];
	//	if(mask_val >= 0.5f)
		{
		//	auto const count_before = std::size(set);
			set.erase_one(source(src_x, src_y));
		/*	if(count_before == std::size(set))
			{ fprintf(stderr, "Not erased: %u %u %.8e %zu\n", src_x, src_y, source(src_x, src_y), std::size(set)); }
			else
			{
				printf("Erased: %u %u %.8e %zu\n", src_x, src_y, source(src_x, src_y), std::size(set));
			}*/
		}
	}
}

void insertCol(Texpainter::PreallocatedMultiset<float>& set,
	MaskOrigin origin,
	Texpainter::Span2d<float const> source,
	float const* mask,
	BoundingBox bb,
	uint32_t mask_x)
{
	for(uint32_t mask_y = bb.y_min; mask_y != bb.y_max; ++mask_y)
	{
		auto const src_x = (origin.x + mask_x + source.width())%source.width();
		auto const src_y = (origin.y + mask_y + source.height())%source.height();
		auto const mask_val = mask[mask_y * source.width() + mask_x];
		if(mask_val >= 0.5f)
		{
		//	printf("Insert: %u %u %.8e %zu\n", src_x, src_y, source(src_x, src_y), std::size(set) + 1);
		//	fflush(stdout);
			set.insert(source(src_x, src_y));
		}
	}
}

void fill(Texpainter::PreallocatedMultiset<float>& set,
		  MaskOrigin origin,
          Texpainter::Span2d<float const> source,
          float const* mask,
          BoundingBox bb)
{
	for(uint32_t mask_y = bb.y_min; mask_y != bb.y_max; ++mask_y)
	{
		for(uint32_t mask_x = bb.x_min; mask_x != bb.x_max; ++mask_x)
		{
			auto const src_x = (origin.x + mask_x + source.width())%source.width();
			auto const src_y = (origin.y + mask_y + source.height())%source.height();
			auto const mask_val = mask[mask_y * source.width() + mask_x];
			if(mask_val >= 0.5f)
			{
		//	printf("Insert: %u %u %.8e %zu\n", src_x, src_y, source(src_x, src_y), std::size(set) + 1);
		//	fflush(stdout);
			set.insert(source(src_x, src_y));
			}
		}
	}
}

void main(auto const& args, auto const& params)
{
	auto const pixel_count = area(args.canvasSize());
	std::ranges::fill(std::span{output<0>(args), pixel_count}, 0.0f);

	auto sorted_vals       = createMultiset(std::span{input<1>(args), pixel_count});
	auto const size        = args.canvasSize();
	auto const bb          = computeBoundingBox(Texpainter::Span2d{input<1>(args), size});

	auto const rx = static_cast<int32_t>(size.width())/2;
	auto const ry = static_cast<int32_t>(size.height())/2;
	auto const source = Texpainter::Span2d{input<0>(args), size};
	auto const mask = input<1>(args);
	auto const minmax = param<Str{"Minmax"}>(params).value();

	for(uint32_t y = 0; y != size.height(); ++y)
	{
		fill(sorted_vals, MaskOrigin{-rx, static_cast<int>(y) - ry}, source, mask, bb);
		output<0>(args, 0, y) = std::lerp(sorted_vals.min(), sorted_vals.max(), minmax);
		auto x_prev = 0;
		for(uint32_t x = 1; x != size.width(); ++x)
		{
			auto const count_before = std::size(sorted_vals);
			eraseCol(sorted_vals, MaskOrigin{x_prev - rx, static_cast<int32_t>(y) - ry}, source, mask, bb, bb.x_min);
			insertCol(sorted_vals, MaskOrigin{static_cast<int32_t>(x) - rx, static_cast<int32_t>(y) - ry}, source, mask, bb, bb.x_max - 1);
			printf("%u %u %zu %zu\n", x, y, count_before, std::size(sorted_vals));
			output<0>(args, x, y) = std::lerp(sorted_vals.min(), sorted_vals.max(), minmax);
			x_prev = x;
		}
		sorted_vals.clear();
	}
}
```

## Tags

__Id:__ f5898fc46c3d7f70c7c8489a40b84b0d

__Category:__ Filters

__Release state:__ Stable