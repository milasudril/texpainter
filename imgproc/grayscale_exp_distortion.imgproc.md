# Grayscale exp distortion

This image processor takes a grayscale image, and transforms its pixel values by a power function.

$$ g(z, y) = 2^{r(f(x, y) - 1)} $$

## Input ports

__Input:__ (Grayscale image) The input image

## Output ports

__Ouptut:__ (Grayscale image) The output image

## Parameters

__Rate:__ (= 0.5)

## Implementation

__Includes:__ 

```c++
#include <cmath>
```

__Source code:__ 

```c++
inline auto rate(ParamValue val) { return std::exp2(std::lerp(-4.0, 4.0, val.value())); }

void main(auto const& args, auto const& params)
{
	auto const r = rate(param<Str{"Rate"}>(params));

	std::transform(input<0>(args),
	               input<0>(args) + area(args.canvasSize()),
	               output<0>(args),
	               [r](auto val) { return std::exp2(r * (val - 1)); });
}
```

## Tags

__Id:__ 080280794b59d25715d14b5efc55b41c

__Category:__ Dynamic range adjustments

__Release state:__ Stable