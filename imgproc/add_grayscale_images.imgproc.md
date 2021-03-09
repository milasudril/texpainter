# Add grayscale images

This image processor takes two grayscale images `A` and `B` and computes their pixel-wise sum. The output `Sum` is the pixel-wise sum of `A` and `B`.

## Input ports

__A:__ (Grayscale image) First operand

__B:__ (Grayscale image) Second operand

## Output ports

__Sum:__ (Grayscale image) The sum of `A` and `B`

## Parameters

__Gain A:__ (= 0.5) Amplification factor for `A`, between -8.0 evFS and +8.0 evFS. 0.5 maps to 0 evFS.

__Gain B:__ (= 0.5) Amplification factor for `B`, between -8.0 evFS and +8.0 evFS. 0.5 maps to 0 evFS.

## Implementation

__Includes:__ 

```c++
#include <algorithm>
#include <cmath>
```

__Source code:__ 

```c++
inline double mapParameter(ParamValue val) { return std::exp2(std::lerp(-8.0, 8.0, val.value())); }

void main(auto const& args, auto const& params)
{
	auto const size = area(args.canvasSize());
	std::transform(input<0>(args),
	               input<0>(args) + size,
	               input<1>(args),
	               output<0>(args),
	               [gain_a = mapParameter(param<Str{"Gain A"}>(params)),
	                gain_b = mapParameter(param<Str{"Gain B"}>(params))](auto a, auto b) {
		               return gain_a * a + gain_b * b;
	               });
}
```

## Tags

__Id:__ 50b448d8742d60fb2bf7b1ef8be2b8da

__Category:__ Arithmetic operators