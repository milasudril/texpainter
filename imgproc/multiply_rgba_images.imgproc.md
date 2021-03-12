# Multiply RGBA images

This image processor takes two RGBA images `A` and `B` and computes their pixel-wise product. The output `Product` is the pixel-wise product of `A` and `B`.

## Input ports

__A:__ (RGBA image) First operand

__B:__ (RGBA image) Second operand

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

__Id:__ f783eb4ef84945821e4479ed61da1e86

__Category:__ Arithmetic operators

__Release state:__ Stable