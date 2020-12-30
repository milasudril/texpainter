# Butterworth decay

This image processor takes a grayscale image, and transforms its pixels according to the equation

$$ g(x, y) = \frac{1}{\sqrt{1 + f(x, y)^n}} $$

## Input ports

__Input:__ (Grayscale image) The input image

## Output ports

__Output:__ (Grayscale image) The output image

## Parameters

__Order:__ (= 0.0) The order $n$. 0.0 maps to 1 and 1.0 maps to 4.

## Implementation

__Includes:__ 

```c++
#include <cmath>
#include <algorithm>
```

__Source code:__ 

```c++
inline auto order(ParamValue val)
{
	return static_cast<int>(std::nextafter(4.0, 0.0) * val.value()) + 1;
}

void main(auto const& args, auto const& params)
{
	auto const size = area(args.size());
	std::transform(input<0>(args),
	               input<0>(args) + size,
	               output<0>(args),
	               [n = order(param<Str{"Order"}>(params))](auto val) {
		               return 1.0 / sqrt(1 + std::pow(val, n));
	               });
}
```

## Tags

__Id:__ f975a41e0ce2ead49231271c4ed43eea

__Category:__ Dynamic range adjustments