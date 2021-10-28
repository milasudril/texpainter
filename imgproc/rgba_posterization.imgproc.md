# RGBA posterization

This image processor applies a posterization filter to its input

## Input ports

__Input:__ (RGBA image) The input image

## Output ports

__Ouptut:__ (RGBA image) The output image

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
using vec4i_t = int __attribute__((vector_size(16)));

constexpr auto toInts(Texpainter::vec4_t vals)
{
	return vec4i_t{static_cast<int>(vals[0]),
	               static_cast<int>(vals[1]),
	               static_cast<int>(vals[2]),
	               static_cast<int>(vals[3])};
}

constexpr auto toFloats(vec4i_t vals)
{
	return Texpainter::vec4_t{static_cast<float>(vals[0]),
	                          static_cast<float>(vals[1]),
	                          static_cast<float>(vals[2]),
	                          static_cast<float>(vals[3])};
}

auto log2vec(Texpainter::vec4_t vals)
{
	return Texpainter::vec4_t{
	    std::log2(vals[0]), std::log2(vals[1]), std::log2(vals[2]), std::log2(vals[3])};
}

auto exp2vec(Texpainter::vec4_t vals)
{
	return Texpainter::vec4_t{
	    std::exp2(vals[0]), std::exp2(vals[1]), std::exp2(vals[2]), std::exp2(vals[3])};
}

constexpr auto minval()
{
	return toFloats(vec4i_t{std::numeric_limits<float>::min_exponent,
	                        std::numeric_limits<float>::min_exponent,
	                        std::numeric_limits<float>::min_exponent,
	                        std::numeric_limits<float>::min_exponent});
}

constexpr auto maxvec(Texpainter::vec4_t a, Texpainter::vec4_t b) { return a > b ? a : b; }

constexpr auto minvec(Texpainter::vec4_t a, Texpainter::vec4_t b) { return a < b ? a : b; }

constexpr auto ones = Texpainter::vec4_t{1.0f, 1.0f, 1.0f, 1.0f};

void main(auto const& args, auto const& params)
{
	constexpr auto MaxMultiplier =
	    std::log2(static_cast<double>((1 << std::numeric_limits<float>::digits) - 1));
	constexpr auto MinMultiplier = std::log2(1.0);

	auto const multiplier_exp = std::lerp(
	    MinMultiplier, MaxMultiplier, static_cast<double>(param<Str{"#Steps"}>(params).value()));

	auto const N  = static_cast<int>(std::exp2(multiplier_exp));
	auto const dx = 1.0f / static_cast<float>(N);


	if(param<Str{"Mode"}>(params).value() < 0.5)
	{
		std::transform(input<0>(args),
		               input<0>(args) + area(args.canvasSize()),
		               output<0>(args),
		               [mult = dx, N](auto val) {
			               auto const val_new =
			                   mult * toFloats(toInts(val.value() * static_cast<float>(N) + 0.5f));
			               return RgbaValue{minvec(val_new, ones)};
		               });
	}
	else
	{
		std::transform(
		    input<0>(args),
		    input<0>(args) + area(args.canvasSize()),
		    output<0>(args),
		    [mult = dx, N](auto val) {
			    auto const transformed_val = (maxvec(log2vec(val.value()), minval()) - minval())
			                                 / (std::log2(1.0f) - minval());

			    auto const rounded_val =
			        mult * toFloats(toInts(transformed_val * static_cast<float>(N) + 0.5f));

				auto const val_new = exp2vec(rounded_val * (std::log2(1.0f) - minval()) + minval());

			    return RgbaValue{minvec(val_new, ones)};
		    });
	}
}
```

## Tags

__Id:__ e52ae993a22f3a9f9330c735cd78d82e

__Category:__ Dynamic range adjustments

__Release state:__ Stable