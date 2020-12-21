# Polygon mask

## Output ports

__Output:__ (Grayscale image) Output image

## Parameters

__Number of vertices:__ (= 0.0) The number of vertices. 0.0 maps to four vertices. 1.0 maps to 12.

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
	RealValue ret = 0;
	for(int k = 0; k < n; ++k)
	{
		vec2_t vert{std::cos(2 * std::numbers::pi * k / n), std::sin(2 * std::numbers::pi * k / n)};
		ret = std::max(ret, std::abs(Texpainter::dot(loc, vert)));
	}
	return ret;
}

void main(auto const& args, auto const& params)
{
	auto const w = args.size().width();
	auto const h = args.size().height();
	auto const r = 0.5 * sqrt(args.size().area());

	auto O       = 0.5 * vec2_t{static_cast<double>(w), static_cast<double>(h)};
	auto const n = 2
	               * static_cast<int>(std::lerp(
	                   2, std::nextafter(7, 0), param<Str{"Number of vertices"}>(params).value()));

	for(uint32_t y = 0; y < h; ++y)
	{
		for(uint32_t x = 0; x < w; ++x)
		{
			output<0>(args, x, y) =
			    value(vec2_t{static_cast<double>(x), static_cast<double>(y)} - O, n) / r;
		}
	}
}
```

## Tags

__Id:__ fc8f077d53cf4e48b05b8a66a34c7bd4

__Category:__ Generators