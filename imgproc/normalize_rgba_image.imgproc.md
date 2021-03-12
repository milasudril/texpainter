# Normalize RGBA image

This image processor takes a grayscale image, computes its minimum and maximum values, and makes these values correspond to the `Output min` and `Output max` parameters.

## Input ports

__Input:__ (RGBA image) The input image

## Output ports

__Output:__ (RGBA image) The output image

## Parameters

__Output min:__ (= 0.0) The minum value in the output image

__Output max:__ (= 1.0) The maximum value in the output image

## Implementation

__Includes:__ 

```c++
#include <algorithm>
```

__Source code:__ 

```c++
float mapIntensity(ParamValue val)
{
	return static_cast<float>(std::exp2(16.0 * (val.value() - 1.0)));
}

void main(auto const& args, auto const& params)
{
	auto const size       = area(args.canvasSize());
	auto const input_view = std::span{input<0>(args), input<0>(args) + size};
	auto const range      = std::pair{
        std::ranges::min_element(input_view, [](auto a, auto b) { return min(a) < min(b); }),
        std::ranges::max_element(input_view, [](auto a, auto b) { return max(a) < max(b); })};

	auto const min_in_val = min(*range.first);
	auto const max_in_val = max(*range.second);

	auto const min_out_val = mapIntensity(param<Str{"Output min"}>(params));
	auto const max_out_val = mapIntensity(param<Str{"Output max"}>(params));

	std::ranges::transform(
	    input_view,
	    output<0>(args),
	    [min_in  = RgbaValue{min_in_val, min_in_val, min_in_val, 0.0f},
	     max_in  = RgbaValue{max_in_val, max_in_val, max_in_val, 1.0f},
	     min_out = RgbaValue{min_out_val, min_out_val, min_out_val, 0.0f},
	     max_out = RgbaValue{max_out_val, max_out_val, max_out_val, 1.0f}](auto val) {
		    auto const t = (val - min_in) / (max_in - min_in);
		    return t * max_out + (RgbaValue{1.0f, 1.0f, 1.0f, 1.0f} - t) * min_out;
	    });
}
```

## Tags

__Id:__ 26dd5c24346ae05540c6c59947440863

__Category:__ Dynamic range adjustments

__Release state:__ Stable