# Grayscale dither

## Input ports

__Input:__ (Grayscale image)

## Output ports

__Output:__ (Grayscale image)

## Parameters

__Intensity:__ (= 1.0)

## Implementation

__Includes:__

```c++
#include <algorithm>
#include <random>
```

__Source code:__

```c++
void main(auto const& args, auto const& params)
{
	std::transform(input<0>(args),
	               input<0>(args) + area(args.canvasSize()),
	               output<0>(args),
	               [intensity = std::exp2(std::lerp(-14.0f, 0.0f, param<Str{"Intensity"}>(params).value())),
	                rng       = Rng{args.rngSeed()}] (auto value) mutable {
	                return std::bernoulli_distribution{value*intensity}(rng);
	                });
}
```

## Tags

__Id:__ 3ef380a94ddd9c6e9cd4ad50e50068c2

__Category:__ Dynamic range adjustments

__Release state:__ Experimental