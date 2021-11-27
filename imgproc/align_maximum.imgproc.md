# Align maximum

This image processor takes a grayscale image and translates its pixels such that the highest intensity is moved to a specific location.

## Input ports

__Input:__ (Grayscale image) Input image

## Output ports

__Output:__ (Grayscale image) Output image

## Parameters

__x:__ (= 0) The x coordinate

__y:__ (= 0) The y coordinate

## Implementation

__Source code:__ 

```c++
inline int32_t mapParameter(ParamValue value, uint32_t scale)
{
	return static_cast<int32_t>(std::lerp(0.0f, 1.0f, value.value()) * static_cast<float>(scale));
}

void main(auto const& args, auto const& params)
{
	auto const w = args.canvasSize().width();
	auto const h = args.canvasSize().height();

	auto const x = mapParameter(param<Str{"x"}>(params), w);
	auto const y = mapParameter(param<Str{"y"}>(params), h);

	auto x_0 = 0u;
	auto y_0 = 0u;
	auto max = input<0>(args, x_0, y_0);

	for(uint32_t row = 0; row < h; ++row)
	{
		for(uint32_t col = 0; col < w; ++col)
		{
			if(auto val = input<0>(args, col, row); val > max)
			{
				max = val;
				x_0 = col;
				y_0 = row;
			}
		}
	}

	auto const Δx = x - x_0;
	auto const Δy = y - y_0;

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

__Id:__ b96719034b15d949439fbce04f28996e

__Category:__ Spatial transformations

__Release state:__ Stable