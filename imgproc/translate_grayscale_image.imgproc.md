# Translate grayscale image

This image processor takes an grayscale image and translates its pixels with a given offset. Boundary conditions are set to periodic. That is, if a part of the source image would end up outside the image, it will wrap aruond to the other edge.

## Input ports

__Input:__ (Grayscale image) Input image

## Output ports

__Output:__ (Grayscale image) Output image

## Parameters

__Δx:__ (= 0.5) x offset. The value 0.0 maps to -0.5 and 1.0 maps to 0.5.

__Δy:__ (= 0.5) y offset. The value 0.0 maps to -0.5 and 1.0 maps to 0.5.

## Implementation

__Source code:__ 

```c++
inline int32_t mapParameter(ParamValue value, uint32_t scale)
{
	return static_cast<int32_t>(std::lerp(-0.5, 0.5, value.value()) * scale);
}

void main(auto const& args, auto const& params)
{
	auto const w = args.size().width();
	auto const h = args.size().height();

	auto const Δx = mapParameter(param<Str{"Δx"}>(params), w);
	auto const Δy = mapParameter(param<Str{"Δy"}>(params), w);

	for(uint32_t row = 0; row < h; ++row)
	{
		for(uint32_t col = 0; col < w; ++col)
		{
			output<0>(args, col, row) = input<0>(args, ((w + col) - Δx) % w, ((h + row) + Δy) % h);
		}
	}
}
```

## Tags

__Id:__ 2a54afec8ce138f2596b09c024f0e7a0

__Category:__ Spatial transformations