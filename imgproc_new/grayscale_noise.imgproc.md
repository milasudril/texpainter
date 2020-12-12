# Grayscale noise

This image processor generates grayscale white noise.

## Input ports

This image processor has no input ports.

## Output ports

__Intensity:__ (Grayscale image) The generated intensity function

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
	auto const size = args.size().area();
	std::generate(output<0>(args),
	              output<0>(args) + size,
	              [U   = std::uniform_real_distribution{0.0, 1.0},
	               rng = Texpainter::DefaultRng::engine()]() mutable { return U(rng); });
}
```

## Tags

__Id:__ 5d03472443bfc964fea98df862b20bf6

__Category:__ Generators