# Soft clamp RGBA image

## Input ports

__Input:__ (RGBA image) The input image

## Output ports

__Ouptut:__ (RGBA image) The output image

## Parameters

__Input min:__ (= 0.0) The minimum value in the input image. Values below this value are clamped to zero.

__Input max:__ (= 1.0) The maximum value in the output image. Values below this value are clamped to one.

__Softness:__ (= 0.0) Clipping softness. If zero, min and max are connected linearly, which implies that there is a discontinuity in the derivative. If set to one, a cubic spline with zero derivative in the endpoints is used. Any value in between blends between these two curves. Note, because a zero derivative in the endpoints forces the maximum slope to be steeper, a higher softness may look sharper.

## Implementation

__Source code:__ 

```c++
inline RgbaValue computeValue(RgbaValue x, RgbaValue min, RgbaValue max, float softness)
{
	auto const xi           = ((2 * x - min - max) / (max - min)).value();
	auto const linear_model = 0.5f * (xi + 1.0f);
	auto const cubic_model  = (xi * (3 - xi * xi) + 2.0f) / 4.0f;
	return RgbaValue{
	    xi <= -1.0f
	        ? 0.0f
	        : (xi >= 1.0f ? 1.0f : (1.0f - softness) * linear_model + softness * cubic_model)};
}

void main(auto const& args, auto const& params)
{
	auto const x_min    = static_cast<float>(param<Str{"Input min"}>(params).value());
	auto const x_max    = static_cast<float>(param<Str{"Input max"}>(params).value());
	auto const softness = static_cast<float>(param<Str{"Softness"}>(params).value());

	std::transform(input<0>(args),
	               input<0>(args) + area(args.size()),
	               output<0>(args),
	               [min = RgbaValue{x_min, x_min, x_min, 0.0},
	                max = RgbaValue{x_max, x_max, x_max, 1.0},
	                softness](auto val) { return computeValue(val, min, max, softness); });
}
```

## Tags

__Id:__ 15f89706a8302f699badc3cdc0cce0ec

__Category:__ Dynamic range adjustments