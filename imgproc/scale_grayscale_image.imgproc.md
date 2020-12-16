# Scale grayscale image

## Input ports

__Input:__ (Grayscale image)

## Output ports

__Output:__ (Grayscale image)

## Parameters

__Scale factor:__ (= 1.0)

__Horz scale factor:__ (= 1.0)

__Vert scale factor:__ (= 1.0)

__Fill mode:__ (= 0.0)

## Implementation

__Includes:__

```c++
#include "utils/rect.hpp"
```

__Source code:__

```c++
inline double mapParameter(ParamValue value) { return std::lerp(-1.0, 1.0, value.value()); }

void main(auto const& args, auto const& params)
{
	using Texpainter::vec2_t;

	auto const w        = args.size().width();
	auto const h        = args.size().height();
	auto const size_vec = vec2_t{static_cast<double>(w), static_cast<double>(h)};

	auto const O = 0.5 * vec2_t{static_cast<double>(w), static_cast<double>(h)};

	auto const r   = mapParameter(param<Str{"Scale factor"}>(params));
	auto const r_x = r * mapParameter(param<Str{"Horz scale factor"}>(params));
	auto const r_y = r * mapParameter(param<Str{"Vert scale factor"}>(params));

	auto const s = vec2_t{r_x, r_y};

	auto const bounding_box = 0.5 * Texpainter::axisAlignedBoundingBox(s * size_vec);
	auto const begin_coords = O - bounding_box;
	auto const end_coords   = O + bounding_box;

	for(int row = static_cast<int>(begin_coords[1]); row < static_cast<int>(end_coords[1]); ++row)
	{
		for(int col = static_cast<int>(begin_coords[0]); col < static_cast<int>(end_coords[0]);
		    ++col)
		{
			auto const loc   = vec2_t{static_cast<double>(col), static_cast<double>(row)};
			auto const src   = O + (loc - O) / s;
			auto const src_x = std::clamp(static_cast<int32_t>(src[0]), 0, static_cast<int32_t>(w - 1));
			auto const src_y = std::clamp(static_cast<int32_t>(src[1]), 0, static_cast<int32_t>(h - 1));
			output<0>(args, col, row) = input<0>(args, src_x, src_y);
		}
	}
}
```

## Tags

__Id:__ 18cb587cdc572bdd556b8a587d9c0f3c

__Category:__ Spatial transformations