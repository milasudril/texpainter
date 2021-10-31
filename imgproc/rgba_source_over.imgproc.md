# RGBA Source over

This image processor implements the alpha over operator. That is

$$\text{Output} = \text{Source} + \text{Destination}\times\left(1 - \text{Source}_\alpha \right)$$

## Input ports

__Source:__ (RGBA image) The source image

__Destination:__ (RGBA image) The destination image

## Output ports

__Output:__ (RGBA image) The composed image

## Parameters

__Mul src by α:__ (= 0.0) Set to multiply source RGB values by source alpha

__Mul dest by α:__ (= 0.0) Set to multiply destination RGB values by source alpha

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

	auto const mode =
	    static_cast<int>(param<Str{"Mul src by α"}>(params).value() >= 0.5f)
	    | (static_cast<int>(param<Str{"Mul dest by α"}>(params).value() >= 0.5f) << 1);

	switch(mode)
	{
		case 0:
			std::transform(input<0>(args),
			               input<0>(args) + size,
			               input<1>(args),
			               output<0>(args),
			               [](auto src, auto dest) { return src + dest * (1.0f - src.alpha()); });
			break;
		case 1:
			std::transform(
			    input<0>(args),
			    input<0>(args) + size,
			    input<1>(args),
			    output<0>(args),
			    [](auto src, auto dest) {
				    auto const output_alpha = src.alpha() + dest.alpha() * (1.0f - src.alpha());
				    auto const src_val      = src.value();
				    auto const src_alpha    = src.alpha();
				    auto const dest_val     = dest.value();
				    auto output_rgb         = src_alpha * src_val + (1.0f - src_alpha) * dest_val;

				    return RgbaValue(output_rgb[0], output_rgb[1], output_rgb[2], output_alpha);
			    });
			break;
		case 2:
			std::transform(
			    input<0>(args),
			    input<0>(args) + size,
			    input<1>(args),
			    output<0>(args),
			    [](auto src, auto dest) {
				    auto const output_alpha = src.alpha() + dest.alpha() * (1.0f - src.alpha());
				    auto const src_val      = src.value();
				    auto const src_alpha    = src.alpha();
				    auto const dest_val     = dest.value();
				    auto const dest_alpha   = dest.alpha();
				    auto output_rgb         = src_val + dest_alpha * (1.0f - src_alpha) * dest_val;

				    return RgbaValue(output_rgb[0], output_rgb[1], output_rgb[2], output_alpha);
			    });
			break;
		case 3:
			std::transform(
			    input<0>(args),
			    input<0>(args) + size,
			    input<1>(args),
			    output<0>(args),
			    [](auto src, auto dest) {
				    auto const output_alpha = src.alpha() + dest.alpha() * (1.0f - src.alpha());
				    auto const src_val      = src.value();
				    auto const src_alpha    = src.alpha();
				    auto const dest_val     = dest.value();
				    auto const dest_alpha   = dest.alpha();
				    auto output_rgb =
				        src_alpha * src_val + dest_alpha * (1.0f - src_alpha) * dest_val;

				    return RgbaValue(output_rgb[0], output_rgb[1], output_rgb[2], output_alpha);
			    });
			break;
	}
}
```

## Tags

__Id:__ b65334b771e511c1e4cc198d60ef792a

__Category:__ Arithmetic operators

__Release state:__ Stable