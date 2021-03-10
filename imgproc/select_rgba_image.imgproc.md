# Select RGBA image

This image processor uses a grayscale image to select one of three rgba images.

## Input ports

__Selector:__ (Grayscale image) The image used to select image

__Image 1:__ (RGBA image) The first image

__Image 2:__ (RGBA image) The second image

__Image 3:__ (RGBA image) The the third image

## Output ports

__Result:__ (RGBA image) The generated image

## Parameters

__Stop 1:__ (= 0.33) Transition point from 1 to 2

__Stop 2:__ (= 0.66) Transition point from 2 to 3

## Implementation

__Source code:__ 

```c++
void main(auto const& args, auto const& params)
{
	auto const stop_1 = param<Str{"Stop 1"}>(params).value();
	auto const stop_2 = std::max(param<Str{"Stop 2"}>(params).value(), stop_1);
	auto const w      = args.canvasSize().width();
	auto const h      = args.canvasSize().height();

	for(uint32_t row = 0; row < h; ++row)
	{
		for(uint32_t col = 0; col < w; ++col)
		{
			auto const val = input<0>(args, col, row);
			if(val < stop_1) { output<0>(args, col, row) = input<1>(args, col, row); }
			else if(val < stop_2)
			{
				output<0>(args, col, row) = input<2>(args, col, row);
			}
			else
			{
				output<0>(args, col, row) = input<3>(args, col, row);
			}
		}
	}
}
```

## Tags

__Id:__ c5875442ab982ed46d74bc68d78a616e

__Category:__ Converters