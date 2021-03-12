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
#include "pixel_store/imgtransform.hpp"

#include <numbers>
```

__Source code:__ 

```c++
using Texpainter::Angle;
using Texpainter::Span2d;
using Texpainter::vec2_t;
using Texpainter::PixelStore::renderCentered;
using Texpainter::PixelStore::renderTiled;

inline Angle angle(ParamValue val)
{
	return Angle{std::lerp(-0.5, 0.5, val.value()), Angle::Turns{}};
}

void main(auto const& args, auto const& params)
{
	auto const ϴ = angle(param<Str{"Angle"}>(params));
	if(param<Str{"Fill mode"}>(params).value() < 0.5)
	{ renderCentered(Span2d{input<0>(args), args.canvasSize()}, output<0>(args), ϴ); }
	else
	{
		renderTiled(Span2d{input<0>(args), args.canvasSize()}, output<0>(args), ϴ);
	}
}
```

## Tags

__Id:__ 1227304b94eaf3c2db36165b2c8ae0f2

__Category:__ Spatial transformations

__Release state:__ Stable