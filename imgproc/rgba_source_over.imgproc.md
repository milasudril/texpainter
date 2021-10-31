# RGBA Source over

This image processor implements the alpha over operator. That is

$$\text{Output} = \text{Source} + \text{Destination}\times\left(1 - \text{Source}_\alpha \right)$$

## Input ports

__Source:__ (RGBA image) The source image

__Destination:__ (RGBA image) The destination image

## Output ports

__Output:__ (RGBA image) The composed image

## Parameters

__Straight α:__ (= 0.0) Set to greater than 0.5 to use straight alpha compositing

## Implementation

__Includes:__

```c++
#include <algorithm>
```

__Source code:__

```c++
void main(auto const& args, auto const& params)
{
	auto const size = area(args.canvasSize());
	if(param<Str{"Straight α"}>(params).value() < 0.5f)
	{
		std::transform(input<0>(args),
					input<0>(args) + size,
					input<1>(args),
					output<0>(args),
					[](auto src, auto dest) { return src + dest * (1.0f - src.alpha()); });
	}
	else
	{
		std::transform(input<0>(args),
				input<0>(args) + size,
				input<1>(args),
				output<0>(args),
				[](auto src, auto dest) {
				auto output_alpha = src.alpha() + dest.alpha()*(1.0f - src.alpha());
				if(output_alpha == 0.0f) [[unlikely ]] { return RgbaValue{0.0f, 0.0f, 0.0f, 0.0f}; }

				auto const src_val = src.value();
				auto const src_alpha = src.alpha();
				auto const dest_val = dest.value();
				auto const dest_alpha = dest.alpha();
				auto output_rgb = (src_alpha*src_val + dest_alpha*(1.0f - src_alpha)*dest_val)/
					output_alpha;

				return RgbaValue(output_rgb[0], output_rgb[1], output_rgb[2], output_alpha);
		});
	}
}
```

## Tags

__Id:__ b65334b771e511c1e4cc198d60ef792a

__Category:__ Arithmetic operators

__Release state:__ Stable