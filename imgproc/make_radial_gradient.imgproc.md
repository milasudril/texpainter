# Make radial gradient

This image processor generates a radial gradient from black to white, with black in the origin. The
pixel values are proportional to the square of the distance to the origin. "Distance" can be chosen
such that the gradient is circular, or forms an even-sided polygon, with the number of vertices in
the range 4 to 12.

## Output ports

__Output:__ (Grayscale image) Output image

## Parameters

__Size:__ (= 0.9333333333333333) The size of the mask, along the nominal x axis. The default value
is set such that the diameter is halve the size of the image.

__Scale with resolution:__ (= 1.0) If > 0.5, scale the size with rendering resolution. Enable this
on for use with spectral filtering.

__Aspect ratio:__ (= 1.0) The aspect ratio of mask. 1.0 means that the mask is circular.

__Orientation:__ (= 0.0) Orientation of the mask. 1.0 maps to π.

__Number of vertices:__ (= 1.0) The number of vertices. 0.0 maps to four vertices. 1.0 makes a
circle. The largest number of vertices before the polygon turning into a circle is 12.

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

void main(auto const& args, auto const& params)
{
	auto const w = args.canvasSize().width();
	auto const h = args.canvasSize().height();

	auto const r_x =
	    0.5 * sizeFromMin(args.canvasSize(), param<Str{"Size"}>(params))
	    / (param<Str{"Scale with resolution"}>(params).value() > 0.5 ? args.resolution() : 1.0);

	auto const r_y = r_x * sizeScaleFactor(param<Str{"Aspect ratio"}>(params));
	auto const θ   = Angle{0.5 * param<Str{"Orientation"}>(params).value(), Angle::Turns{}};
	auto const n   = static_cast<int>(std::lerp(
        0, std::nextafter(std::size(norms2), 0), param<Str{"Number of vertices"}>(params).value()));

	auto const r_0       = vec2_t{r_x, r_y};
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

## TODO:s

* Add mode which fits the long diameter in addition to the short diameter