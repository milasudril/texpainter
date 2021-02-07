# Place RGBA image at points

This image processor places a RGBA image at location deterimed by the input point cloud.

## Input ports

__Locations:__ (Point cloud) The probability distribution.

__Image:__ (RGBA image) the image to place at the points

## Output ports

__Result:__ (RGBA image) The generated image

## Implementation

__Source code:__

```c++
void main(auto const& args)
{
	std::ranges::for_each(input<0>(args).get(), [&args](auto pos) {

		auto const w = args.canvasSize().width();
		auto const h = args.canvasSize().height();

		auto const Δx = pos.x;
		auto const Δy = pos.y;

		for(uint32_t row = 0; row < h; ++row)
		{
			for(uint32_t col = 0; col < w; ++col)
			{
				auto src = input<1>(args, ((w + col) - Δx) % w, ((h + row) - Δy) % h);
				output<0>(args, col, row) = (1.0f - src.alpha()) * output<0>(args, col, row) + src.alpha()*src;
			}
		}
	});

	std::ranges::for_each(input<0>(args).get(), [&args](auto pos) {
		output<0>(args, pos.x, pos.y) = RgbaValue{1.0f, 1.0f, 1.0f, 1.0f};
	});
}
```

## Tags

__Id:__ f872b1455c56665160d5faa3e0eae98c

__Category:__ Converters