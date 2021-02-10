# Grayscale power distortion

This image processor takes a grayscale image, and transforms its pixel values by a power function.

$$ g(z, y) = z(x, y)^a $$

## Input ports

__Input:__ (Grayscale image) The input image

## Output ports

__Ouptut:__ (Grayscale image) The output image

## Parameters

__Exponent:__ (= 0.5) 0.5 maps to $a$ = 1.

## Implementation

__Includes:__

```c++
#include <cmath>
```

__Source code:__

```c++
inline auto power(ParamValue val) { return std::exp2(std::lerp(-2.0, 2.0, val.value())); }

void main(auto const& args, auto const& params)
{
	auto const a = power(param<Str{"Exponent"}>(params));

	std::transform(input<0>(args),
	               input<0>(args) + area(args.canvasSize()),
	               output<0>(args),
	               [a](auto val) { return std::pow(val, a); });
}
```

## Tags

__Id:__ 1593892b70359b6cf53c08d7fe37840f

__Category:__ Dynamic range adjustments