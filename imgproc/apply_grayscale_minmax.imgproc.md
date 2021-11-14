# Apply grayscale minmax

This image processor computes a linear combination of a running min and max filter, given the domain specified by Mask.

## Input ports

__Source:__ (Grayscale image) The source image

__Mask:__ (Grayscale image) The mask to be used for the image

## Output ports

__Min:__ (Grayscale image) The min values

__Max:__ (Grayscale image) The max values

## Parameters

__Mask quant mode:__ (= 0.0) The method used to quantize the mask. 0.0 means using a fixed 0.5 threshould. 1.0 means using a Bernoulli distribution with the mask value as probability.

## Implementation

__Includes:__ 

```c++
#include "rolling_rank_filter/minmax_filter.hpp"

#include <algorithm>
#include <chrono>
```

__Source code:__ 

```c++
void main(auto const& args, auto const& params)
{
	auto const size = args.canvasSize();

	auto const t0 = std::chrono::steady_clock::now();
	if(param<Str{"Mask quant mode"}>(params).value() >= 0.5f)
	{
		Texpainter::RollingRankFilter::minmaxFilter(
		    Texpainter::Span2d{args.template input<0>(), size},
		    args.template input<1>(),
		    args.template output<0>(),
		    args.template output<1>(),
		    args.rngSeed());
	}
	else
	{
		Texpainter::RollingRankFilter::minmaxFilter(
		    Texpainter::Span2d{args.template input<0>(), size},
		    args.template input<1>(),
		    args.template output<0>(),
		    args.template output<1>());
	}
	auto const t1 = std::chrono::steady_clock::now();
	printf("%zu\n", (t1 - t0).count());
}
```

## Tags

__Id:__ f5898fc46c3d7f70c7c8489a40b84b0d

__Category:__ Filters

__Release state:__ Stable