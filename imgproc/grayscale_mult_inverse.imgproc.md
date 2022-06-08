# Grayscale mult. inverse

This image processor takes a grayscale image, and generates its pixel-wise multiplicative inverse, that it

$$ g(z, y) = 1/f(x, y) $$

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
	               [](auto val) { return 1.0f / val; });
}
```

## Tags

__Id:__ 78c2b91f098d2b3ea52641b5d4889b11

__Category:__ Dynamic range adjustments

__Release state:__ Stable