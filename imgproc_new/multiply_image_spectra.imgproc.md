# Multiply image spectra

This image processor takes two image spectra `A` and `B` and computes their pixel-wise product. The output `Product` is a new spectrum that is the pixel-wise product of `A` and `B`.

## Input ports

__A:__ (Image spectrum) First operand

__B:__ (Image spectrum) Second operand

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
void main(auto const& args)
{
	auto const size = args.size().area();
	std::transform(
	    input<0>(args), input<0>(args) + size, input<1>(args), output<0>(args), std::multiplies{});
}
```

## Tags

__Id:__ c0bf15ee911dba0e5f45a9bb85e16802

__Category:__ Arithmetic operators

