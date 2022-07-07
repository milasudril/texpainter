# Separate magnitude and direction

This image processor takes a vector field and separates its magnitude and direction

## Input ports

__Input:__ (Vector field) The input image

## Output ports

__r:__ (Grayscale image) The magnitude, that is $\sqrt{x^2 + y^2}$

__φ:__ (Grayscale image) The direction, as if computed by `std::atan2(y, x)`

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
			auto const val = input<0>(args, col, row);
			output<0>(args, col, row) = static_cast<float>(std::sqrt(Texpainter::dot(val, val)));
			output<1>(args, col, row) = static_cast<float>(std::atan2(val[1], val[0]));
		}
	}
}
```

## Tags

__Id:__ e4ab1e1aaa85e7464f374b0c191da6e3

__Category:__ Converters

__Release state:__ Stable