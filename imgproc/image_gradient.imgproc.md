# Image gradient

This image processor takes a grayscale image and computes its gradient

## Input ports

__Input:__ (Grayscale image) The input image

## Output ports

__Output:__ (Vector field) The gradient (as a vector field)

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
			auto const dx = input<0>(args, (col + 1 + w)%w,  row)
				- input<0>(args, (col - 1 + w)%w,  row);
			auto const dy = input<0>(args, col,  (row + 1 + h)%h)
				- input<0>(args, col, (row - 1 + h)%h);
			output<0>(args, col, row) = 0.5*vec2_t{dx, dy};
		}
	}
}
```

## Tags

__Id:__ f95cba837f25f1937364b50b75b18b5d

__Category:__ Converters

__Release state:__ Stable