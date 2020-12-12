# Multiply image spectrum and grayscale image

This image processor takes one image spectrum `A`, and one grayscale image `B`, and computes their pixel-wise product. The output `Product` is a new spectrum that is the pixel-wise product of `A` and `B`.

## Input ports

__A:__ (Image spectrum) First operand

__B:__ (Grayscale real pixels) Second operand

## Output ports

__Product:__ (Image spectrum) The product of `A` and `B`

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

__Id:__ b50bf360be9b511bf8bc6129956f0158

__Category:__ Arithmetic operators

