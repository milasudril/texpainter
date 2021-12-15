# Blackbody

This image processor takes a grayscale image that represents temperature, and maps it to the color
of a blackbody radiator.

## Input ports

__Temp:__ (Grayscale image) Absolute temperature. 0.0f is mapped to 0 K. 1.0f is mapped to 16384 K.

## Output ports

__Output:__ (RGBA image) The output image

## Parameters

__Scale intensity:__ (= 0.0) Set to greater than 0.5 to use scale intensity by $$T^4$$.

## Implementation

__Includes:__

```c++
#include <algorithm>
```

__Source code:__

```c++
void main(auto const& args, auto const& params)
{
	auto size = area(args.canvasSize());
	if(auto const t4 = param<Str{"Scale intensity"}>(params).value(); t4 >= 0.5f)
	{
		std::transform(input<0>(args), input<0>(args) + size, output<0>(args), [](auto const val){
			return WavelengthConv::blackbodyIntensity(val)*WavelengthConv::blackbodyColor(val);
		});
	}
	else
	{
		std::transform(input<0>(args), input<0>(args) + size, output<0>(args), [](auto const val){
			return WavelengthConv::blackbodyColor(val);
		});
	}
}
```

## Tags

__Id:__ cee86d9d9ef61783386b3f72971a778f

__Category:__ Converters

__Release state:__ Stable