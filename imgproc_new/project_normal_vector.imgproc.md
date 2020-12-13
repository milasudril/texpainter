# Project normal vector

This image processor takes the normal vector (possibly computed by \ref{Make topographic map}), and projects int onto another vector.

## Input ports

__Input:__ (Topography data)

## Output ports

__Output:__ (Grayscale image) The result of the projection. Notice that it is possible that the output contains negative values.

## Parameters

__Azimuth angle:__ (= 0.0) The aziumth angle, measured in turns

__Zenith angle:__ (= 0.0) The zenith angle, measured in quater-turns

## Implementation

__Source code:__ 

```c++
using Texpainter::vec4_t;

void main(auto const& arg, auto const& params)
{
	auto const phi   = 2.0 * std::numbers::pi * param<Str{"Azimuth angle"}>(params).value();
	auto const theta = 0.5 * std::numbers::pi * param<Str{"Zenith angle"}>(params).value();
	vec4_t proj{static_cast<float>(sin(theta) * cos(phi)),
	            static_cast<float>(sin(theta) * sin(phi)),
	            static_cast<float>(cos(theta)),
	            0.0f};

	std::transform(input<0>(arg),
	               input<0>(arg) + arg.size().area(),
	               output<0>(arg),
	               [proj](auto val) { return Texpainter::dot(val.value(), proj); });
}
```

## Tags

__Id:__ dd205464d683126a9da4972613e849e4

__Category:__ Converters