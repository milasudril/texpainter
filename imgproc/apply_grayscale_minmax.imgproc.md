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
#include "utils/span_2d.hpp"
#include "pixel_store/image.hpp"

#include <algorithm>
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

using Mask = Texpainter::PixelStore::Image<char>;

auto crop(Texpainter::Span2d<float const> img, BoundingBox bb)
{
	auto const width  = bb.x_max - bb.x_min;
	auto const height = bb.y_max - bb.y_min;
	Mask ret{width, height};
	size_t N = 0;
	for(auto y = bb.y_min; y != bb.y_max; ++y)
	{
		for(auto x = bb.x_min; x != bb.x_max; ++x)
		{
			N += static_cast<size_t>(img(x, y) >= 0.5f);
			ret(x - bb.x_min, y - bb.y_min) = img(x, y) >= 0.5f ? 1 : 0;
		}
	}

	return std::pair{std::move(ret), N};
}

void main(auto const& args)
{
	auto const size    = args.canvasSize();
	auto const mask_bb = computeBoundingBox(Texpainter::Span2d{input<1>(args), size});
	auto const mask    = crop(Texpainter::Span2d{input<1>(args), size}, mask_bb);
	auto const w       = size.width();
	auto const h       = size.height();
	auto const w_m     = mask.first.width();
	auto const h_m     = mask.first.height();

	auto const masked_pixels = std::make_unique<float[]>(mask.second);

	for(auto y = 0u; y != h; ++y)
	{
		for(auto x = 0u; x != w; ++x)
		{
			auto masked_pixels_end = masked_pixels.get();
			for(auto y_m = 0u; y_m != h_m; ++y_m)
			{
				for(auto x_m = 0u; x_m != w_m; ++x_m)
				{
					if(mask.first(x_m, y_m) == 1)
					{
						*masked_pixels_end = input<0>(
						    args, (x + x_m + w - w_m / 2) % w, (y + y_m + h - h_m / 2) % h);
						++masked_pixels_end;
					}
				}
			}
			auto const m = masked_pixels.get() + mask.second / 2;
			std::nth_element(masked_pixels.get(), m, masked_pixels_end);
			output<0>(args, x, y) = *m;
		}
	}
}
```

## Tags

__Id:__ f5898fc46c3d7f70c7c8489a40b84b0d

__Category:__ Filters

__Release state:__ Stable