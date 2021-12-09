# Single spawn spot

## Output ports

__Output:__ (Point cloud)

## Parameters

__x:__ (= 0.5)

__y:__ (= 0.5)

__Rotation:__ (= 0.0)

__Scale:__ (= 1.0)

## Implementation

__Includes:__ 

```c++
#include <random>
#include <deque>
```

__Source code:__ 

```c++
void main(auto const& args, auto const& params)
{
	auto const w     = static_cast<float>(args.canvasSize().width());
	auto const h     = static_cast<float>(args.canvasSize().height());
	auto const x     = static_cast<uint32_t>(param<Str{"x"}>(params).value() * w);
	auto const y     = static_cast<uint32_t>(param<Str{"y"}>(params).value() * h);
	auto const rot   = Angle{param<Str{"Rotation"}>(params).value(), Angle::Turns{}};
	auto const scale = param<Str{"Scale"}>(params).value();

	output<0>(args).get() = std::vector{SpawnSpot{ImageCoordinates{x, y}, rot, scale}};
}
```

## Tags

__Id:__ cd33cdab1eaca76db4ff5186f416e843

__Category:__ Generators

__Release state:__ Stable