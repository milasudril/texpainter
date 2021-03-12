# Multiply RGBA and grayscale images

This image processor takes one RGBA image `A`, and one grayscale image `B`, and computes their pixel-wise product. The output `Product` is the pixel-wise product of `A` and `B`.

## Input ports

__A:__ (RGBA image) First operand

__B:__ (Grayscale image) Second operand

## Output ports

__Product:__ (RGBA image) The product of `A` and `B`

## Implementation

__Includes:__ 

```c++
#include <algorithm>
#include <functional>
```

__Source code:__ 

```c++
inline void main(auto const& args)
{
	auto const size = area(args.canvasSize());
	std::transform(
	    input<0>(args), input<0>(args) + size, input<1>(args), output<0>(args), std::multiplies{});
}
```

## Tags

__Id:__ 478f69b7dafaa1594c345df283d82021

__Category:__ Arithmetic operators

__Release state:__ Stable