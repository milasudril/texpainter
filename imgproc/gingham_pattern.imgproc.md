# Gingham pattern

This image processor generates a gingham pattern

## Input ports

This image processor has no input ports.

## Output ports

__Intensity:__ (Grayscale image) The generated intensity function

## Parameters

__Div x:__ (= 0.75) The number of cells per pattern period

__Scale with resolution:__ (= 0.0) If > 0.5, scale the size with rendering resolution. Use for spectral filtering.

__Aspect ratio:__ (= 1.0) The aspect ratio of cells. 1.0 means that they are squares

## Implementation

__Includes:__ 

```c++
#include <cmath>
```

__Source code:__ 

```c++
constexpr RealValue pattern[2][2] = {0.0, 0.5, 0.5, 1.0};

void main(auto const& args, auto const& params)
{
	auto const size = args.canvasSize();
	auto const A    = area(size);
	auto const w    = args.canvasSize().width();
	auto const h    = args.canvasSize().height();

	auto const dx =
	    2.0 * static_cast<int>(0.5 * toDiameter(size, param<Str{"Div x"}>(params).value()))
	    / (sqrt(A)
	       * (param<Str{"Scale with resolution"}>(params).value() < 0.5 ? args.resolution() : 1.0));
	auto const dy = dx * param<Str{"Aspect ratio"}>(params).value();

	for(uint32_t row = 0; row < h; ++row)
	{
		auto const i = static_cast<uint32_t>(row * dy);
		for(uint32_t col = 0; col < w; ++col)
		{
			auto const j = static_cast<uint32_t>(col * dx);

			output<0>(args, col, row) = pattern[i % 2][j % 2];
		}
	}
}
```

## Tags

__Id:__ 32f4d34efe492c5880313a6df71b617d

__Category:__ Generators