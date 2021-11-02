# Apply grayscale minmax

This image processor computes a linear combination of a running min and max filter, given the domain
specified by Mask.

## Input ports

__Source:__ (Grayscale image) The source image

__Mask:__ (Grayscale image) The mask to be used for the image

## Output ports

__Output:__ (Grayscale image) The filtered image

## Implementation

__Includes:__

```c++
#include "utils/preallocated_multiset.hpp"

#include <algorithm>
```

__Source code:__

```c++
void main(auto const&)
{
}
```

## Tags

__Id:__ f5898fc46c3d7f70c7c8489a40b84b0d

__Category:__ Filters

__Release state:__ Stable