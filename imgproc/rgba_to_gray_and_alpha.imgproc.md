# RGBA to Gray and Alpha

This image processor takes an RGBA image and generates two grayscale images. The first output is the total intensity computed from a weighted sum of the red, green, and blue channels. The second image contains the original alpha channe. The intensity of the first image is normailzed. This means that if all weights are set to the same value, the weights used for the intensity calculations will be $1/3$ for all channels.

## Input ports

__Input:__ (RGBA image) The image to convert

## Output ports

__Intensity:__ (Grayscale image) The computed intesity values

__Alpha:__ (Grayscale image) The alpha channel of the input, as is

## Parameters

__Red:__ (= 1.0) The amount of red to be used in the output

__Green:__ (= 1.0) The amount of green to be used in the output

__Blue:__ (= 1.0) The amount of blue to be used in the output

## Implementation

__Includes:__ 

```c++
#include <numeric>
```

__Source code:__ 

```c++
using Texpainter::vec4_t;

vec4_t computeWeights(std::array<ParamValue, 3> const& vals)
{
	std::array<double, 3> weights;
	// Reverse order because vals is sorted by param name, and Blue is before Red
	std::ranges::transform(vals, std::rbegin(weights), [](auto val) { return val.value(); });
	auto const sum = std::accumulate(std::begin(weights), std::end(weights), 0.0);
	if(sum == 0.0) { return vec4_t{1.0f, 1.0f, 1.0f, 0.0f} / 3.0f; }
	return vec4_t{static_cast<float>(weights[0]),
	              static_cast<float>(weights[1]),
	              static_cast<float>(weights[2]),
	              0.0f}
	       / static_cast<float>(sum);
}

void main(auto const& args, auto const& params)
{
	std::transform(
	    input<0>(args),
	    input<0>(args) + area(args.canvasSize()),
	    output<0>(args),
	    [weights = computeWeights(params.values())](auto val) {
		    auto weighted_val = val.value() * weights;
		    return std::clamp(
		        weighted_val[0] + weighted_val[1] + weighted_val[2] + weighted_val[3], 0.0f, 1.0f);
	    });

	std::transform(input<0>(args),
	               input<0>(args) + area(args.canvasSize()),
	               output<1>(args),
	               [](auto val) { return val.alpha(); });
}
```

## Tags

__Id:__ e87d6316bbcbde15f0543d66c79b9c75

__Category:__ Converters