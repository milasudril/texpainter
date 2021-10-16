# Make linear gradient

This image processor generates a linear gradient

## Output ports

__Output:__ (Grayscale image)

## Parameters

__Orientation:__ (= 0.0) Orientation of the gradient. 1.0 maps to 2π.

__Size:__ (= 1.0) The size of the gradient, along the nominal x axis.

## Implementation

__Source code:__ 

```c++
void main(auto const& args, auto const& params)
{
	auto const w     = args.canvasSize().width();
	auto const h     = args.canvasSize().height();
	auto const theta = Angle{param<Str{"Orientation"}>(params).value(), Angle::Turns{}};
	auto const O     = 0.5 * vec2_t{static_cast<double>(w), static_cast<double>(h)};
	auto const r_0   = 0.5f * sizeFromMin(args.canvasSize(), param<Str{"Size"}>(params));

	for(uint32_t row = 0; row < h; ++row)
	{
		for(uint32_t col = 0; col < w; ++col)
		{
			auto const pos  = vec2_t{static_cast<double>(col), static_cast<double>(row)} - O;
			auto const n_xy = vec2_t{cos(theta), sin(theta)};
			output<0>(args, row, col) = static_cast<RealValue>(Texpainter::dot(pos, n_xy)) / r_0;
		}
	}
}
```

## Tags

__Id:__ d5024afd1121398e571ab0c29962d0cb

__Category:__ Generators

__Release state:__ Stable