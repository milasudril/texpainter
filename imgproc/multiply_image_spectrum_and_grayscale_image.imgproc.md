# Multiply image spectrum and grayscale image

This image processor takes one image spectrum `A`, and one grayscale image `B`, and computes their pixel-wise product. The output `Product` is a new spectrum that is the pixel-wise product of `A` and `B`.

## Input ports

__A:__ (Image spectrum) First operand

__B:__ (Grayscale image) Second operand

## Output ports

__Product:__ (Image spectrum) The product of `A` and `B`

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

__Id:__ b50bf360be9b511bf8bc6129956f0158

__Category:__ Arithmetic operators

__Release state:__ Stable