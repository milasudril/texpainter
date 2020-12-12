# Mix image spectra

## Input ports

__A:__ (Image spectrum)

__B:__ (Image spectrum)

__Factor:__ (Grayscale image)

## Output ports

__Output:__ (Image spectrum)

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

__Id:__ 2f173bff22705eed2a7fd39757d07166

__Category:__ Arithmetic operators