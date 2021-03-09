# Checkerboard pattern

This image processor generates a classic checkerboard pattern.

## Input ports

This image processor has no input ports.

## Output ports

__Intensity:__ (Grayscale image) The generated intensity function

## Parameters

__Wavelength:__ (= 0.8666666666666667) Wavelength. The default value is set such that it results in a wavelength of $1/4$ of the image size.

__Aspect ratio:__ (= 1.0) The aspect ratio of cells. 1.0 means that they are squares

## Implementation

__Includes:__ 

```c++
#include <cmath>
```

__Source code:__ 

```c++
void main(auto const& args, auto const& params)
{
	auto const w = args.canvasSize().width();
	auto const h = args.canvasSize().height();

	auto const f  = 2.0 / sizeFromWidth(args.canvasSize(), param<Str{"Wavelength"}>(params));
	auto const fx = f;
	auto const fy = fx * w / sizeFromHeight(args.canvasSize(), param<Str{"Aspect ratio"}>(params));

	for(uint32_t row = 0; row < h; ++row)
	{
		auto const i = static_cast<uint32_t>(row * fy);
		for(uint32_t col = 0; col < w; ++col)
		{
			auto const j              = static_cast<uint32_t>(col * fx);
			output<0>(args, col, row) = (i % 2 == j % 2) ? 0.0 : 1.0;
		}
	}
}
```

## Tags

__Id:__ 133c5997f18cfa25316c73b1dc17cfeb

__Category:__ Generators