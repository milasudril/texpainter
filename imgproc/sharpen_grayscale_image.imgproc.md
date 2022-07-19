# Sharpen grayscale image

## Input ports

__Input:__ (Grayscale image) The input image

## Output ports

__Output:__ (Grayscale image) The output channel

## Parameters

__Strength:__ (= 0.5) The strength of the filter

## Implementation

__Source code:__ 

```c++
void main(auto const& args, auto const& params)
{
	auto const size = args.canvasSize();

	auto const center    = std::exp2(8.0f * param<Str{"Strength"}>(params).value());
	auto const neighbour = 0.25f * (1.0f - center);

	std::array<std::array<float, 3>, 3> const weights{
	    std::array<float, 3>{0.0f, neighbour, 0.0f},
	    std::array<float, 3>{neighbour, center, neighbour},
	    std::array<float, 3>{0.0f, neighbour, 0.0f}};

	auto const w = size.width();
	auto const h = size.height();

	for(uint32_t row = 0; row != h; ++row)
	{
		for(uint32_t col = 0; col != w; ++col)
		{
			auto sum = 0.0f;
			for(uint32_t k = 0; k != std::size(weights); ++k)
			{
				for(uint32_t l = 0; l != std::size(weights[k]); ++l)
				{
					sum += input<0>(args, (col + w + l - 1) % w, (row + k + h - 1) % h)
					       * weights[k][l];
				}
			}
			output<0>(args, col, row) = sum;
		}
	}
}
```

## Tags

__Id:__ 73596dadf9aef71aa7adca81252ad6d1

__Category:__ Filters

__Release state:__ Stable