# Scale RGBA image

This image processor scales a RGBA image by a given factor between -1.0 and 1.0.

## Input ports

__Input:__ (RGBA image) The input image

## Output ports

__Output:__ (RGBA image) The output image

## Parameters

This image processor takes four parameters. Three of these are three parameters that affect the scale factor, `Scale factor`, `Horz scale factor`, and `Vert scale factor`. The effective horizontal scale factor is `Scale factor` × `Horz scale factor`. The effective vertical horizontal scale factor is `Scale factor` × `Vert scale factor`. Notice that a negative value can be set if it is desired to mirror the image.

__Scale factor:__ (= 1.0) Isotropic scale factor

__Horz scale factor:__ (= 1.0) Horizontal scale factor

__Vert scale factor:__ (= 1.0) Vertical scale factor

__Fill mode:__ (= 0.0) The method to be used when painting outside the resulting image. If smaller than 0.5, the values outside the resulting image will be zero. Otherwise, the resulting image will be tiled.

## Implementation

__Includes:__ 

```c++
#include "utils/rect.hpp"
```

__Source code:__ 

```c++
using Texpainter::vec2_t;

inline double mapParameter(ParamValue value) { return std::lerp(-1.0, 1.0, value.value()); }

inline void renderTiled(auto const& args, vec2_t s)
{
	auto const w = args.size().width();
	auto const h = args.size().height();

	auto const O = 0.5 * vec2_t{static_cast<double>(w), static_cast<double>(h)};

	for(uint32_t row = 0; row < h; ++row)
	{
		for(uint32_t col = 0; col < w; ++col)
		{
			auto const loc            = vec2_t{static_cast<double>(col), static_cast<double>(row)};
			auto const src            = O + (loc - O) / s;
			auto const src_x          = static_cast<int32_t>(src[0]);
			auto const src_y          = static_cast<int32_t>(src[1]);
			output<0>(args, col, row) = input<0>(args, (w + src_x) % w, (h + src_y) % h);
		}
	}
}

inline void renderCentered(auto const& args, vec2_t s)
{
	auto const w = args.size().width();
	auto const h = args.size().height();

	auto const O = 0.5 * vec2_t{static_cast<double>(w), static_cast<double>(h)};

	auto const size_vec     = vec2_t{static_cast<double>(w), static_cast<double>(h)};
	auto const bounding_box = 0.5 * Texpainter::axisAlignedBoundingBox(s * size_vec);
	auto const begin_coords = O - bounding_box;
	auto const end_coords   = O + bounding_box;

	for(int row = static_cast<int>(begin_coords[1]); row < static_cast<int>(end_coords[1]); ++row)
	{
		for(int col = static_cast<int>(begin_coords[0]); col < static_cast<int>(end_coords[0]);
		    ++col)
		{
			auto const loc = vec2_t{static_cast<double>(col), static_cast<double>(row)};
			auto const src = O + (loc - O) / s;
			auto const src_x =
			    std::clamp(static_cast<int32_t>(src[0]), 0, static_cast<int32_t>(w - 1));
			auto const src_y =
			    std::clamp(static_cast<int32_t>(src[1]), 0, static_cast<int32_t>(h - 1));
			output<0>(args, col, row) = input<0>(args, src_x, src_y);
		}
	}
}

void main(auto const& args, auto const& params)
{
	auto const r   = mapParameter(param<Str{"Scale factor"}>(params));
	auto const r_x = r * mapParameter(param<Str{"Horz scale factor"}>(params));
	auto const r_y = r * mapParameter(param<Str{"Vert scale factor"}>(params));
	auto const s   = vec2_t{r_x, r_y};

	if(param<Str{"Fill mode"}>(params).value() < 0.5) { renderCentered(args, s); }
	else
	{
		renderTiled(args, s);
	}
}
```

## Tags

__Id:__ 85378f5367b9bfd4b94097cca01ee315

__Category:__ Spatial transformations