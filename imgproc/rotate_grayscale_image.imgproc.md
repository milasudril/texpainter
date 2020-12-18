# Rotate grayscale image

This image processor rotates a grayscale image by a given angle.

## Input ports

__Input:__ (Grayscale image) The input image

## Output ports

__Output:__ (Grayscale image) The output image

## Parameters

__Angle:__ (= 0.5) The rotation angle

__Fill mode:__ (= 0.0) The method to be used when painting outside the resulting image. If smaller than 0.5, the values outside the resulting image will be zero. Otherwise, the resulting image will be tiled.

## Implementation

__Includes:__

```c++
#include "utils/rect.hpp"
#include "utils/vec_t.hpp"

#include <numbers>
```

__Source code:__

```c++
using Texpainter::vec2_t;

inline double angle(ParamValue val)
{
	return std::numbers::pi*std::lerp(-1, 1, val.value());
}

void main(auto const& args, auto const& params)
{
	auto const ϴ            = angle(param<Str{"Angle"}>(params));
	auto const rot_x        = vec2_t{cos(ϴ), sin(ϴ)};
	auto const rot_y        = vec2_t{-sin(ϴ), cos(ϴ)};
	auto const size = args.size();
	auto const size_vec = vec2_t{static_cast<double>(size.width()), static_cast<double>(size.height())};

	auto const O = 0.5 * size_vec;

	for(uint32_t row = 0; row < size.height(); ++row)
	{
		for(uint32_t col = 0; col < size.width(); ++col)
		{
			auto const loc_ret = vec2_t{static_cast<double>(col), static_cast<double>(row)};
			auto const src_pos = Texpainter::transform(loc_ret - O, rot_x, rot_y) + O;
			auto src_x = static_cast<int32_t>(src_pos[0]);
			auto src_y = static_cast<int32_t>(src_pos[1]);
/*			if(src_pos[0] >= 0 && src_pos[0] < size.width() && src_pos[1] >= 0
			   && src_pos[1] < size.height()) [[likely]]*/
			{
				output<0>(args, col, row) = input<0>(args, (src_x + size.width())%size.width(), (src_y + size.height())%size.height());
			}
		}
	}

}
```

## Tags

__Id:__ 1227304b94eaf3c2db36165b2c8ae0f2

__Category:__ Spatial transformations