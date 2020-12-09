# Multiply grayscale images

This image processor takes two grayscale images `A` and `B` and computes their product. The output
`Product` is the product of `A` and `B`.

## Input ports

__A:__ (Grayscale real pixels) First operand

__B:__ (Grayscale real pixels) Second operand

## Output ports

__Product:__ (Grayscale real pixels) The product of `A` and `B`

## Implementation

To loop through all pixels in `A` and `B`, `std::transform` is used. As callback to
`std::transform`, `std::multiplies` is used.

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
	std::transform(input<0>(args),
	               input<0>(args) + size,
	               input<1>(args),
	               output<0>(args),
	               std::multiplies{});
}
```

## Tags

__Id:__ 487b4182c3b147b1fa04162cf9c17723

__Category:__ Arithmetic operators