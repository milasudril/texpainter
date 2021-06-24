# Select max Grayscale

This image processor takes the largest pixels from A or B, and assigs it to the output image.

## Input ports

__A:__ (Grayscale image)

__B:__ (Grayscale image)

## Output ports

__Output:__ (Grayscale image)

## Implementation

__Includes:__ 

```c++
#include <algorithm>
```

__Source code:__ 

```c++
void main(auto const& args)
{
	auto const size = area(args.canvasSize());

	std::transform(
	    input<0>(args), input<0>(args) + size, input<1>(args), output<0>(args), [](auto a, auto b) {
		    return std::max(a, b);
	    });
}
```

## Tags

__Id:__ 45474f44e8180b3990fc63c98e5e7e8a

__Category:__ Masking operations

__Release state:__ Stable