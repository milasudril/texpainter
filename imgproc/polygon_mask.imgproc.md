# Polygon mask

## Output ports

__Output:__ (Grayscale image) Output image

## Parameters

__Number of vertices:__ (= 1.0) The number of vertices. 0.0 maps to four vertices. 1.0 makes a circle. The largest number of vertices before the polygon turning into a circle is 12.

## Implementation

__Includes:__

```c++
#include "utils/vec_t.hpp"

#include <cmath>
#include <numbers>
```

__Source code:__

```c++
template<int n>
inline RealValue polygon(vec2_t loc)
{
	RealValue ret = 0;
	for(int k = 0; k < n; ++k)
	{
		vec2_t vert{std::cos(2 * std::numbers::pi * k / n), std::sin(2 * std::numbers::pi * k / n)};
		ret = std::max(ret, std::abs(Texpainter::dot(loc, vert)));
	}
	return ret * ret;
}

inline RealValue circle(vec2_t loc) { return Texpainter::dot(loc, loc); }

using NormSquared = RealValue (*)(vec2_t loc);

constexpr NormSquared norms2[] = {
    polygon<4>, polygon<6>, polygon<8>, polygon<10>, polygon<12>, circle};

void main(auto const& args, auto const& params)
{
	auto const w = args.size().width();
	auto const h = args.size().height();
	auto const r = 0.25 * args.size().area();

	auto O           = 0.5 * vec2_t{static_cast<double>(w), static_cast<double>(h)};
	auto const n     = static_cast<int>(std::lerp(
        0, std::nextafter(std::size(norms2), 0), param<Str{"Number of vertices"}>(params).value()));
	auto const norm2 = norms2[n];
	for(uint32_t y = 0; y < h; ++y)
	{
		for(uint32_t x = 0; x < w; ++x)
		{
			output<0>(args, x, y) = norm2(vec2_t{static_cast<double>(x), static_cast<double>(y)} - O) / r;
		}
	}
}
```

## Tags

__Id:__ fc8f077d53cf4e48b05b8a66a34c7bd4

__Category:__ Generators