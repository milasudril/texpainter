# Grayscale sqrt distortion

This image processor takes a grayscale image, and transforms its pixel values the square root function.

$$ g(z, y) = \sqrt{f(x, y)} $$

## Input ports

__Input:__ (Grayscale image) The input image

## Output ports

__Ouptut:__ (Grayscale image) The output image

## Implementation

__Includes:__ 

```c++
#include <cmath>
```

__Source code:__ 

```c++
void main(auto const& args)
{
	std::transform(input<0>(args),
	               input<0>(args) + area(args.canvasSize()),
	               output<0>(args),
	               [](auto val) { return std::sqrt(val); });
}
```

## Tags

__Id:__ d188f60e01e9b3ed5377c0443d821f81

__Category:__ Dynamic range adjustments

__Release state:__ Stable