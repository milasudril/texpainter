# Gingham pattern

This image processor generates a gingham pattern

## Input ports

This image processor has no input ports.

## Output ports

__Intensity:__ (Grayscale image) The generated intensity function

## Parameters

__Wavelength:__ (= 0.86666667) Wavelength. The default value is set such that it results in a wavelength of $1/4$ of the image size.

__Aspect ratio:__ (= 1.0) The aspect ratio of cells. 1.0 means that they are squares, in case the canvas is a square.

## Implementation

__Includes:__ 

```c++
#include <cmath>
```

__Source code:__ 

```c++
constexpr RealValue pattern[2][2] = {0.0, 0.5, 0.5, 1.0};

inline double quantize(double val, uint32_t n)
{
	return static_cast<uint32_t>(val * n + 0.5) / static_cast<double>(n);
}

void main(auto const& args, auto const& params)
{
	auto const w = args.canvasSize().width();
	auto const h = args.canvasSize().height();

	auto const f  = 2.0 / sizeFromWidth(args.canvasSize(), param<Str{"Wavelength"}>(params));
	auto const fx = 2.0 * quantize(0.5 * f, w);
	auto const fy =
	    2.0
	    * quantize(
	        0.5 * f * w / sizeFromHeight(args.canvasSize(), param<Str{"Aspect ratio"}>(params)), h);

	for(uint32_t row = 0; row < h; ++row)
	{
		auto const i = static_cast<uint32_t>(row * fy);
		for(uint32_t col = 0; col < w; ++col)
		{
			auto const j = static_cast<uint32_t>(col * fx);

			output<0>(args, col, row) = pattern[i % 2][j % 2];
		}
	}
}
```

## Tags

__Id:__ 32f4d34efe492c5880313a6df71b617d

__Category:__ Generators

__Release state:__ Stable