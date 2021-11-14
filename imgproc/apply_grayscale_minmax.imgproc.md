# Apply grayscale minmax

This image processor computes a linear combination of a running min and max filter, given the domain specified by Mask.

## Input ports

__Source:__ (Grayscale image) The source image

__Mask:__ (Grayscale image) The mask to be used for the image

## Output ports

__Min:__ (Grayscale image) The min values

__Max:__ (Grayscale image) The max values

## Implementation

__Includes:__ 

```c++
#include "rolling_rank_filter/minmax_filter.hpp"

#include <algorithm>
#include <chrono>
```

__Source code:__ 

```c++
void main(auto const& args)
{
	auto const size = args.canvasSize();

	Texpainter::RollingRankFilter::minmaxFilter(Texpainter::Span2d{args.template input<0>(), size},
	                                            args.template input<1>(),
	                                            args.template output<0>(),
	                                            args.template output<1>());
}
```

## Tags

__Id:__ f5898fc46c3d7f70c7c8489a40b84b0d

__Category:__ Filters

__Release state:__ Stable