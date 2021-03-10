# Exponential decay

This image processor takes a grayscale image, and transforms its pixels according to the equation

$$ g(x, y) = 2^{-f(x, y)/2} $$

## Input ports

__Input:__ (Grayscale image) The input image

## Output ports

__Output:__ (Grayscale image) The output image

## Implementation

__Includes:__

```c++
#include <cmath>
#include <algorithm>
```

__Source code:__

```c++
void main(auto const& args)
{
	auto const size = area(args.canvasSize());
	std::transform(input<0>(args), input<0>(args) + size, output<0>(args), [](auto val) {
		return std::exp2(-0.5 * val);
	});
}
```

## Tags

__Id:__ 0f61686ad23f9e02eb57099a62ca980e

__Category:__ Dynamic range adjustments