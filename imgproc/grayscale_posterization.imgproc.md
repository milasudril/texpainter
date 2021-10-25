# Grayscale posterization

This image processor applies a posterization filter to its input

## Input ports

__Input:__ (Grayscale image) The input image

## Output ports

__Ouptut:__ (Grayscale image) The output image

## Parameters

__#Steps:__ (= 1.0) Controls the number of quantization levels. 1.0 means no reduction in steps

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
	constexpr auto MaxMultiplier =
	    std::log2(static_cast<double>((1 << std::numeric_limits<float>::digits) - 1));
	constexpr auto MinMultiplier = std::log2(1.0);

	auto const multiplier =
		static_cast<int>(std::exp2(std::lerp(
	    MinMultiplier, MaxMultiplier, static_cast<double>(param<Str{"#Steps"}>(params).value()))));

	printf("%d\n", multiplier);

	std::transform(input<0>(args),
	               input<0>(args) + area(args.canvasSize()),
	               output<0>(args),
	               [multiplier](auto val) {
		               return static_cast<float>(static_cast<int>(static_cast<double>(val) * multiplier + 0.5)
		                                         / static_cast<double>(multiplier));
	               });
}
```

## Tags

__Id:__ 5d7c3f15723884d709b7d6a8a46402c8

__Category:__ Dynamic range adjustments

__Release state:__ Stable