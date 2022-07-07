# Separate XY

This image processor takes a vector field and separates its X and Y components

## Input ports

__Input:__ (Vector field) The input image

## Output ports

__x:__ (Grayscale image) The x component

__y:__ (Grayscale image) The y component

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
			output<0>(args, col, row) = static_cast<float>(input<0>(args, col, row)[0]);
			output<1>(args, col, row) = static_cast<float>(input<0>(args, col, row)[1]);
		}
	}
}
```

## Tags

__Id:__ 5e7ad902ab258ec84fe9132db25435a9

__Category:__ Converters

__Release state:__ Stable