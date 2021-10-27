# Grayscale posterization

This image processor applies a posterization filter to its input

## Input ports

__Input:__ (Grayscale image) The input image

## Output ports

__Ouptut:__ (Grayscale image) The output image

## Parameters

__#Steps:__ (= 1.0) Controls the number of quantization levels. 1.0 means no reduction in steps

__Mode:__ (= 0.0) Controls whether or not to use logarithmic quantization

## Implementation

__Includes:__

```c++
#include <cmath>
#include <limits>
```

__Source code:__

```c++
void main(auto const& args, auto const& params)
{
	if(param<Str{"Mode"}>(params).value() < 0.5)
	{
		constexpr auto MaxMultiplier =
		    std::log2(static_cast<double>((1 << std::numeric_limits<float>::digits) - 1));
		constexpr auto MinMultiplier = std::log2(1.0);

		auto const multiplier_exp =
		    std::lerp(MinMultiplier,
		              MaxMultiplier,
		              static_cast<double>(param<Str{"#Steps"}>(params).value()));

		auto const N = static_cast<int>(std::exp2(multiplier_exp));

		auto const dx = 1.0f / static_cast<float>(N);

		std::transform(
		    input<0>(args),
		    input<0>(args) + area(args.canvasSize()),
		    output<0>(args),
		    [mult = dx, N](auto val) {
			    return static_cast<float>(
			        mult * static_cast<int>(val * static_cast<float>(N) + 0.5f));
		    });
	}
	else
	{
		constexpr auto MaxMultiplier =
		    std::log2(static_cast<double>((1 << std::numeric_limits<float>::digits) - 1));
		constexpr auto MinMultiplier = std::log2(1.0);

		auto const multiplier_exp =
		    std::lerp(MinMultiplier,
		              MaxMultiplier,
		              static_cast<double>(param<Str{"#Steps"}>(params).value()));

		auto const N = static_cast<int>(std::exp2(multiplier_exp));

		auto const dx = 1.0f / static_cast<float>(N);

		std::transform(
		    input<0>(args),
		    input<0>(args) + area(args.canvasSize()),
		    output<0>(args),
		    [mult = dx, N](auto val) {
			    constexpr auto MinVal =
			        static_cast<float>(std::numeric_limits<float>::min_exponent);
			    auto const transformed_val =
			        (std::max(std::log2(val), MinVal) - MinVal) / (std::log2(1.0f) - MinVal);

			    auto const rounded_val = static_cast<float>(
			        mult * static_cast<int>(transformed_val * static_cast<float>(N) + 0.5f));

			    return std::exp2(rounded_val * (std::log2(1.0f) - MinVal) + MinVal);
		    });
	}
}
```

## Tags

__Id:__ 5d7c3f15723884d709b7d6a8a46402c8

__Category:__ Dynamic range adjustments

__Release state:__ Stable