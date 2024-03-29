# Random point cloud

This image processor generates a randomized point cloud, given

* a spatial probability distribution

* a rotation

* a size distribution

## Input ports

__Intensity:__ (Grayscale image) The probability distribution

__Rotation:__ (Grayscale image) The rotation distribution

__Scale:__ (Grayscale image) The scale distribution

## Output ports

__Point cloud:__ (Point cloud) The generated point cloud

## Parameters

__Point intensity:__ (= 0.5)

__Volume selection:__ (= 0.0)

## Implementation

__Includes:__ 

```c++
#include <random>
#include <cmath>
```

__Source code:__ 

```c++
void main(auto const& args, auto const& params)
{
	auto& points = output<0>(args).get();
	auto const I = std::exp2(std::lerp(0.0f, 12.0f, param<Str{"Point intensity"}>(params).value()));
	auto const w = args.canvasSize().width();
	auto const h = args.canvasSize().height();
	auto const V =
	    param<Str{"Volume selection"}>(params).value() < 0.5
	        ? static_cast<RealValue>(w * h)
	        : static_cast<RealValue>(std::accumulate(input<0>(args), input<0>(args) + w * h, 0.0));

	std::uniform_real_distribution U{0.0f, V / I};
	auto rng = Rng{args.rngSeed()};

	for(uint32_t row = 0; row != h; ++row)
	{
		for(uint32_t col = 0; col != w; ++col)
		{
			auto val = U(rng);
			if(val <= input<0>(args, col, row))
			{
				points.push_back(SpawnSpot{ImageCoordinates{col, row},
				                           Angle{input<1>(args, col, row), Angle::Turns{}},
				                           input<2>(args, col, row)});
			}
		}
	}
}
```

## Tags

__Id:__ bdb21855bb013b570a0bd47806d65f51

__Category:__ Converters

__Release state:__ Stable