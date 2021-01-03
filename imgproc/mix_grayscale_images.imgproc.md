# Mix Grayscale images

## Input ports

__A:__ (Grayscale image)

__B:__ (Grayscale image)

__Factor:__ (Grayscale image)

## Output ports

__Output:__ (Grayscale image)

## Implementation

__Includes:__ 

```c++
#include "utils/n_ary_transform.hpp"

#include <algorithm>
```

__Source code:__ 

```c++
void main(auto const& args)
{
	auto const size = area(args.canvasSize());
	auto blend      = [](auto a, auto b, auto factor) { return a * (1.0f - factor) + b * factor; };

	Texpainter::transform(blend,
	                      output<0>(args),
	                      input<0>(args),
	                      input<0>(args) + size,
	                      input<1>(args),
	                      input<2>(args));
}
```

## Tags

__Id:__ a3740f87b6396c3d80d5aa736a6fec39

__Category:__ Arithmetic operators