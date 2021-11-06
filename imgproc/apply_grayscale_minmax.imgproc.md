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

using Mask = Texpainter::PixelStore::Image<char>;

inline auto crop(Texpainter::Span2d<float const> img, float threshold)
{
	auto const bb     = computeBoundingBox(img, threshold);
	auto const width  = bb.x_max - bb.x_min;
	auto const height = bb.y_max - bb.y_min;
	Mask ret{width, height};
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

void main(auto const& args)
{
	auto const size = args.canvasSize();
	auto const start_time  = std::chrono::steady_clock::now();
	auto const mask = crop(Texpainter::Span2d{input<1>(args), size}, 0.5f);
	auto const w    = size.width();
	auto const h    = size.height();
	auto const w_m  = mask.first.width();
	auto const h_m  = mask.first.height();

	auto const masked_pixels = std::make_unique<float[]>(mask.second);
	printf("Init %zu\n", (std::chrono::steady_clock::now() - start_time).count());
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
			auto const m = masked_pixels.get();  //+ mask.second / 2;
			std::nth_element(masked_pixels.get(), m, masked_pixels_end);
			output<0>(args, x, y) = *m;
		}
	}
	printf("Render %zu\n", (std::chrono::steady_clock::now() - start_time).count());
}
```

## Tags

__Id:__ f5898fc46c3d7f70c7c8489a40b84b0d

__Category:__ Filters

__Release state:__ Stable