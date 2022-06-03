# Make annulus

## Output ports

__Output:__ (Grayscale image) Output image

## Parameters

__Size:__ (= 0.93333333) The size of the gradient, along the nominal x axis. The default value is set such that the diameter is half the size of the image.

__Scale with resolution:__ (= 1.0) If > 0.5, scale the size with rendering resolution. Enable this when output is used for spectral filtering.

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
		ret = std::max(ret, static_cast<RealValue>(std::abs(Texpainter::dot(loc, vert))));
	}
	return ret * ret;
}

inline RealValue circle(vec2_t loc) { return static_cast<RealValue>(Texpainter::dot(loc, loc)); }

using NormSquared = RealValue (*)(vec2_t loc);

constexpr NormSquared norms2[] = {
    polygon<4>, polygon<6>, polygon<8>, polygon<10>, polygon<12>, circle};

void main(auto const& args, auto const& params)
{
	auto const w = args.canvasSize().width();
	auto const h = args.canvasSize().height();

	auto const scale =
	    static_cast<float>(sizeFromMin(args.canvasSize(), param<Str{"Size"}>(params)));
	auto const r_x = static_cast<float>(
	    0.5 * scale
	    / (param<Str{"Scale with resolution"}>(params).value() > 0.5 ? args.resolution() : 1.0));
	//	auto const annul_size = r_x/scale;

	auto const r_y = r_x * sizeScaleFactor(param<Str{"Aspect ratio"}>(params));
	auto const θ   = Angle{0.5 * param<Str{"Orientation"}>(params).value(), Angle::Turns{}};
	auto const n =
	    static_cast<int>(std::lerp(0.0f,
	                               std::nextafter(static_cast<RealValue>(std::size(norms2)), 0.0f),
	                               param<Str{"Number of vertices"}>(params).value()));

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
			output<0>(args, x, y) = std::abs(std::sqrt(norm2(r)) - 0.5f * r_x);
		}
	}
}
```

## Tags

__Id:__ 1985a21ebf8bf94879420d1acbf8b0e1

__Category:__ Generators

__Release state:__ Experimental

## TODO:s

* Add mode which fits the long diameter in addition to the short diameter