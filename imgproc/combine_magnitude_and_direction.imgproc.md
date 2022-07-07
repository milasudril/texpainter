# Combine magnitude and direction

This image processor combines a magnitude and a direction and generates a vector field

## Input ports

__r:__ (Grayscale image) The magnitude, that is $\sqrt{x^2 + y^2}$

__φ:__ (Grayscale image) The direction, as if computed by `std::atan2(y, x)`

## Output ports

__Output:__ (Vector field) The output image $r(\cos(φ), \sin(φ))$

## Implementation

__Source code:__

```c++
void main(auto const& args)
{
	auto size = args.canvasSize();

	auto const w = size.width();
	auto const h = size.height();

	for(uint32_t row = 0; row != h; ++row)
	{
		for(uint32_t col = 0; col != w; ++col)
		{
			auto const r = input<0>(args, col, row);
			auto const φ = input<1>(args, col, row);
			output<0>(args, col, row) = r*vec2_t{std::cos(φ), std::sin(φ)};
		}
	}
}
```

## Tags

__Id:__ 273f092ffe4c58c7758e7568e39b5f25

__Category:__ Converters

__Release state:__ Stable