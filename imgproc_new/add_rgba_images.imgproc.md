# Add RGBA images

This image processor takes two RGBA images `A` and `B` and computes their pixel-wise sum. The output
`Sum` is the pixel-wise sum of `A` and `B`.

## Input ports

__A:__ (RGBA pixels) First operand

__B:__ (RGBA pixels) Second operand

## Output ports

__Sum:__ (RGBA pixels) The sum of `A` and `B`

## Parameters

__Gain A:__ (= 0.5) Amplification factor for `A`, between -1.0 evFS and +1.0 evFS. 0.5 maps to
0 evFS.

__Gain B:__ (= 0.5) Amplification factor for `B`, between -1.0 evFS and +1.0 evFS. 0.5 maps to
0 evFS.

## Implementation

To loop through all pixels in `A` and `B`, `std::transform` is used. As callback to
`std::transform`, a function object with access to the mapped parameter values are used.
The function object returns a weighted sum of its two arguments, where the weights are
deterimened by the parameters.

__Includes:__

```
#include <algorithm>
#include <cmath>
```

__Source code:__

```c++
inline double mapParameter(ParamValue val)
{
	return std::exp2(std::lerp(-1.0, 1.0, val.value()));
}

void main(auto const& args, auto const& params)
{
	auto const size = args.size().area();
	std::transform(input<0>(args),
	               input<0>(args) + size,
	               input<1>(args),
	               output<0>(args),
	               [gain_a = mapParameter(param<Str{"Gain A"}>(params)),
	               gain_b = mapParameter(param<Str{"Gain B"}>(params))](auto a, auto b) {
	               	return gain_a*a + gain_b*b;
                   });
}
```

## Tags

__Id:__ c5fdebcfc3b0a40c9edcceebacc5d288

__Category:__ Arithmetic operators
