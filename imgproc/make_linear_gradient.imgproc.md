# Make linear gradient

This image processor generates a linear gradient

## Output ports

__Output:__ (Grayscale image)

## Parameters

__Size:__ (= 0.9333333333333333) The size of the mask, along the nominal x axis.

__Orientation:__ (= 0.0) Orientation of the mask. 1.0 maps to 2*π.

## Implementation

__Source code:__

```c++
void main(auto const& args, auto const&)
{
	auto const w = args.canvasSize().width();
	auto const h = args.canvasSize().height();
	for(uint32_t row = 0; row < h; ++row)
	{
		for(uint32_t col = 0; col < w; ++col)
		{
			output<0>(args, row, col) = static_cast<double>(col)/w;
		}
	}
}

```

## Tags

__Id:__ d5024afd1121398e571ab0c29962d0cb

__Category:__ Generators