# Make spiral

## Output ports

__Output:__ (Grayscale image) Output image

## Parameters

__Orientation:__ (= 0.0) Orientation of the mask. 1.0 maps to π.

## Implementation

__Includes:__ 

```c++
#include <cmath>
#include <numbers>
```

__Source code:__ 

```c++
void main(auto const& args, auto const&)
{
	auto const w = args.canvasSize().width();
	auto const h = args.canvasSize().height();
	auto O       = 0.5 * vec2_t{static_cast<double>(w), static_cast<double>(h)};
	for(uint32_t y = 0; y < h; ++y)
	{
		for(uint32_t x = 0; x < w; ++x)
		{
			auto v                = vec2_t{static_cast<double>(x), static_cast<double>(y)} - O;
			output<0>(args, x, y) = std::atan2(v[1], v[0]);
		}
	}
}
```

## Tags

__Id:__ 5b0c3dde49e2fc8bed2bcd0c17b8251a

__Category:__ Generators

__Release state:__ Stable