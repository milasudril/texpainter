# Multiply grayscale images

This image processor takes two grayscale images `A` and `B` and computes their pixel-wise product. The output `Product` is the pixel-wise product of `A` and `B`.

## Input ports

__A:__ (Grayscale image) First operand

__B:__ (Grayscale image) Second operand

## Output ports

__Product:__ (Grayscale image) The product of `A` and `B`

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

__Id:__ 487b4182c3b147b1fa04162cf9c17723

__Category:__ Arithmetic operators