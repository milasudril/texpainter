# Make radial gradient

This image processor generates a radial gradient from black to white, with black in the origin. The pixel values are proportional to the square of the distance to the origin. "Distance" can be chosen such that the gradient is circular, or forms an even-sided polygon, with the number of vertices in the range 4 to 12.

## Output ports

__Output:__ (Grayscale image) Output image

## Parameters

__Size:__ (= 0.5) The size of the mask, along the nominal x axis.

__Aspect ratio:__ (= 1.0) The aspect ratio of mask. 1.0 means that the mask is circular.

__Orientation:__ (= 0.0) Orientation of the mask. 1.0 maps to π.

__Number of vertices:__ (= 1.0) The number of vertices. 0.0 maps to four vertices. 1.0 makes a circle. The largest number of vertices before the polygon turning into a circle is 12.

## Implementation

__Includes:__ 

```c++
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

inline auto size(size_t canvas_size, ParamValue val)
{
	return 0.5 * std::exp2(std::lerp(-std::log2(canvas_size), 0.0, val.value()));
}

void main(auto const& args, auto const& params)
{
	auto const w = args.size().width();
	auto const h = args.size().height();
	auto const A = args.size().area();

	auto const r_x = size(static_cast<size_t>(sqrt(A)), param<Str{"Size"}>(params));
	auto const r_y = r_x * param<Str{"Aspect ratio"}>(params).value();
	auto const θ   = Angle{0.5 * param<Str{"Orientation"}>(params).value(), Angle::Turns{}};
	auto const n   = static_cast<int>(std::lerp(
        0, std::nextafter(std::size(norms2), 0), param<Str{"Number of vertices"}>(params).value()));

	auto const r_0       = sqrt(A) * vec2_t{r_x, r_y};
	auto const rot_vec_x = vec2_t{cos(θ), -sin(θ)};
	auto const rot_vec_y = vec2_t{sin(θ), cos(θ)};
	auto const norm2     = norms2[n];
	auto O               = 0.5 * vec2_t{static_cast<double>(w), static_cast<double>(h)};
	for(uint32_t y = 0; y < h; ++y)
	{
		for(uint32_t x = 0; x < w; ++x)
		{
			auto P                = vec2_t{static_cast<double>(x), static_cast<double>(y)};
			auto r                = Texpainter::transform(P - O, rot_vec_x, rot_vec_y) / r_0;
			output<0>(args, x, y) = norm2(r);
		}
	}
}
```

## Tags

__Id:__ fc8f077d53cf4e48b05b8a66a34c7bd4

__Category:__ Generators