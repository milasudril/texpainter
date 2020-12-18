# Scale grayscale image

This image processor scales a grayscale image by a given factor between -1.0 and 1.0.

## Input ports

__Input:__ (Grayscale image) The input image

## Output ports

__Output:__ (Grayscale image) The output image

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
#include "pixel_store/imgtransform.hpp"
```

__Source code:__ 

```c++
using Texpainter::Span2d;
using Texpainter::vec2_t;
using Texpainter::PixelStore::renderCentered;
using Texpainter::PixelStore::renderTiled;

inline double mapParameter(ParamValue value) { return std::lerp(-1.0, 1.0, value.value()); }

void main(auto const& args, auto const& params)
{
	auto const r   = mapParameter(param<Str{"Scale factor"}>(params));
	auto const r_x = r * mapParameter(param<Str{"Horz scale factor"}>(params));
	auto const r_y = r * mapParameter(param<Str{"Vert scale factor"}>(params));
	auto const s   = vec2_t{r_x, r_y};

	if(param<Str{"Fill mode"}>(params).value() < 0.5)
	{ renderCentered(Span2d{input<0>(args), args.size()}, output<0>(args), s); }
	else
	{
		renderTiled(Span2d{input<0>(args), args.size()}, output<0>(args), s);
	}
}
```

## Tags

__Id:__ 18cb587cdc572bdd556b8a587d9c0f3c

__Category:__ Spatial transformations