# Mix RGBA images

## Input ports

__A:__ (RGBA pixels)

__B:__ (RGBA pixels)

__Factor:__ (Grayscale real pixels)

## Output ports

__Output:__ (RGBA pixels)

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
	auto const size = args.size().area();
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

__Id:__ fe608a2da8ea6e89657409305c76b91c

__Category:__ Arithmetic operators

