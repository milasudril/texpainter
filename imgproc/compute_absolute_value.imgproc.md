# Compute absolute value

This image processor takes a Grayscale image, and computes absolute the absolute value of its pixels.

## Input ports

__Input:__ (Grayscale image) The input image

## Output ports

__Output:__ (Grayscale image) The absolute value

## Implementation

__Source code:__

```c++
void main(auto const& args)
{
	auto const size = args.canvasSize();

	for(uint32_t row = 0; row != size.height(); ++row)
	{
		for(uint32_t col = 0; col != size.width(); ++col)
		{
			output<0>(args, col, row) = std::abs(input<0>(args, col, row));
		}
	}
}
```

## Tags

__Id:__ c380181371a4752d91f259cca8695175

__Category:__ Dynamic range adjustments

__Release state:__ Stable