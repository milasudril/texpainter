# Random point cloud

This image processor generates a randomized point cloud, given a spatial probability distribution

## Input ports

__Intensity:__ (Grayscale image) The probability distribution.

## Output ports

__Point cloud:__ (Point cloud) The generated point cloud

## Parameters

__Intensity gain:__ (= 1.0)

## Implementation

__Includes:__ 

```c++
#include "utils/default_rng.hpp"

#include <random>
```

__Source code:__ 

```c++
void main(auto const& args, auto const& params)
{
	auto& points         = output<0>(args).get();
	auto const intensity = param<Str{"Intensity gain"}>(params).value();
	auto const w         = args.canvasSize().width();
	auto const h         = args.canvasSize().height();
	std::uniform_real_distribution U{0.0, 1.0};
	auto rng = Texpainter::DefaultRng::engine();

	for(uint32_t row = 0; row < h; ++row)
	{
		for(uint32_t col = 0; col < w; ++col)
		{
			auto val = U(rng);
			if(val <= intensity * input<0>(args, col, row))
			{ points.push_back(ImageCoordinates{col, row}); }
		}
	}
}
```

## Tags

__Id:__ bdb21855bb013b570a0bd47806d65f51

__Category:__ Converters