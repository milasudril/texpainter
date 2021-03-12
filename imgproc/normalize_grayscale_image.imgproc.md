# Normalize grayscale image

This image processor takes a grayscale image, computes its minimum and maximum values, and makes these values correspond to the `Output min` and `Output max` parameters.

## Input ports

__Input:__ (Grayscale image) The input image

## Output ports

__Output:__ (Grayscale image) The output image

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
	auto const size  = area(args.canvasSize());
	auto const range = std::minmax_element(input<0>(args), input<0>(args) + size);
	std::transform(input<0>(args),
	               input<0>(args) + size,
	               output<0>(args),
	               [min_in  = *range.first,
	                max_in  = *range.second,
	                min_out = mapIntensity(param<Str{"Output min"}>(params)),
	                max_out = mapIntensity(param<Str{"Output max"}>(params))](auto val) {
		               auto const t = (val - min_in) / (max_in - min_in);
		               return t * max_out + (1.0f - t) * min_out;
	               });
}
```

## Tags

__Id:__ 15cdd6a6bc1a1c9fa1722b6f9150b9de

__Category:__ Dynamic range adjustments

__Release state:__ Stable