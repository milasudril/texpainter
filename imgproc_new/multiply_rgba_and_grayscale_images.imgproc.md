# Multiply RGBA and grayscale images

This image processor takes one RGBA image `A`, and one grayscale image `B`, and computes their pixel-wise product. The output `Product` is the pixel-wise product of `A` and `B`.

## Input ports

__A:__ (RGBA pixels) First operand

__B:__ (Grayscale real pixels) Second operand

## Output ports

__Product:__ (RGBA pixels) The product of `A` and `B`

## Implementation

To loop through all pixels in `A` and `B`, `std::transform` is used. As callback to `std::transform`, `std::multiplies` is used.

__Includes:__ 

```c++
#include <algorithm>
#include <functional>
```

__Source code:__ 

```c++
inline void main(auto const& args)
{
	auto const size = args.size().area();
	std::transform(
	    input<0>(args), input<0>(args) + size, input<1>(args), output<0>(args), std::multiplies{});
}
```

## Tags

__Id:__ 478f69b7dafaa1594c345df283d82021

__Category:__ Arithmetic operators

