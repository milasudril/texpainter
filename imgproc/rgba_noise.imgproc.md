# RGBA noise

This image processor generates RGBA white noise.

## Output ports

__Intensity:__ (RGBA image) The generated intensity function

## Implementation

__Includes:__ 

```c++
#include "utils/default_rng.hpp"

#include <random>
```

__Source code:__ 

```c++
void main(auto const& args)
{
	auto const size = area(args.canvasSize());
	std::generate(output<0>(args),
	              output<0>(args) + size,
	              [U    = std::uniform_real_distribution{0.0f, 1.0f},
	               &rng = Texpainter::DefaultRng::engine()]() mutable {
		              return RgbaValue{U(rng), U(rng), U(rng), U(rng)};
	              });
}
```

## Tags

__Id:__ 8f926e6ad5f96aaebc86f5f8bb70068e

__Category:__ Generators

__Release state:__ Stable