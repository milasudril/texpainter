# Polygon mask

## Output ports

__Output:__ (Grayscale image) Output image

## Implementation

__Includes:__ 

```c++
#include "utils/vec_t.hpp"

#include <cmath>
#include <numbers>
```

__Source code:__ 

```c++
inline RealValue value(vec2_t loc, int n)
{
	RealValue sum = 0;
	for(int k = 0; k < n; ++k)
	{
		vec2_t vert{std::cos(2 * std::numbers::pi * k / n), std::sin(2 * std::numbers::pi * k / n)};
		sum += std::abs(Texpainter::dot(loc, vert));
	}
	return sum;
}

void main(auto const& args)
{
	auto const w = args.size().width();
	auto const h = args.size().height();

	auto O = 0.5 * vec2_t{static_cast<double>(w), static_cast<double>(h)};

	for(uint32_t y = 0; y < h; ++y)
	{
		for(uint32_t x = 0; x < w; ++x)
		{
			output<0>(args, x, y) =
			    value(vec2_t{static_cast<double>(x), static_cast<double>(y)} - O, 6) < 0.5 * w;
		}
	}
}
```

## Tags

__Id:__ fc8f077d53cf4e48b05b8a66a34c7bd4

__Category:__ Generators