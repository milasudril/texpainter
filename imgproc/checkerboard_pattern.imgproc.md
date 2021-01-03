# Checkerboard pattern

This image processor generates a classic checkerboard pattern.

## Input ports

This image processor has no input ports.

## Output ports

__Intensity:__ (Grayscale image) The generated intensity function

## Parameters

__Div x:__ (= 0.5) The number of cells per pattern period

__Aspect ratio:__ (= 1.0) The aspect ratio of cells. 1.0 means that they are squares

## Implementation

__Includes:__ 

```c++
#include <cmath>
```

__Source code:__ 

```c++
inline auto sizeFromParam(size_t size, ParamValue val)
{
	return 0.5 * std::exp2(std::lerp(-std::log2(size), 0.0, val.value()));
}

void main(auto const& args, auto const& params)
{
	auto const w = args.canvasSize().width();
	auto const h = args.canvasSize().height();
	auto dx      = 2.0 * static_cast<int>(w * sizeFromParam(w, param<Str{"Div x"}>(params)))
	          / static_cast<double>(w);
	auto dy = dx * param<Str{"Aspect ratio"}>(params).value();

	for(uint32_t row = 0; row < h; ++row)
	{
		auto const i = static_cast<uint32_t>(row * dy);
		for(uint32_t col = 0; col < w; ++col)
		{
			auto const j              = static_cast<uint32_t>(col * dx);
			output<0>(args, col, row) = (i % 2 == j % 2) ? 0.0 : 1.0;
		}
	}
}
```

## Tags

__Id:__ 133c5997f18cfa25316c73b1dc17cfeb

__Category:__ Generators