# Make RGBA image

This image processor takes four grayscale images, and combines then into one  RGBA image.

## Input ports

__Red:__ (Grayscale image) The `red` channel

__Green:__ (Grayscale image) The `green` channel

__Blue:__ (Grayscale image) The `blue` channel

__Alpha:__ (Grayscale image) The `alpha` channel

## Output ports

__Output:__ (RGBA image) The input image

## Implementation

__Includes:__

```c++
#include "utils/n_ary_transform.hpp"
```

__Source code:__

```c++
void main(auto const& args)
{
	auto size = args.size().area();
	Texpainter::transform(
	    [](auto r, auto g, auto b, auto a) {
		    return RgbaValue{static_cast<float>(r),
		                     static_cast<float>(g),
		                     static_cast<float>(b),
		                     static_cast<float>(a)};
	    },
	    output<0>(args),
	    input<0>(args),
	    input<0>(args) + size,
	    input<1>(args),
	    input<2>(args),
	    input<3>(args));
}
```

## Tags

__Id:__ 2f1a5a2f195dde21a8629191e66fdb2d

__Category:__ Converters