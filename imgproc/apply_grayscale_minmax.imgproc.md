# Apply grayscale minmax

This image processor computes a linear combination of a running min and max filter, given the domain specified by Mask. Pixel values in the mask > 0.5 are included, others are rejected.

## Input ports

__Source:__ (Grayscale image) The source image

__Mask:__ (Grayscale image) The mask to be used for the image

## Parameters

__Minmax:__ (= 0.5) Selects min vs max. 0.0 means min. 1.0 means max. 0.5 will be an approximation of the median.

## Output ports

__Output:__ (Grayscale image) The filtered image

## Implementation

The basic idea behind this implementation is to store pixel values under the mask in a multiset. For each pixel, the leading values are added to the multiset, while the outgoing values are removed. The first pixel will query all pixel values below the mask, following periodic boundary conditions.

To save heap allocations, the multiset is preallocated. In order to compute the maximum size needed for this multiset, the number of white pixels (using the 0.5 threshold) in the mask are countet. To save iterations when probing the mask, its bounding box is used.

__Includes:__ 

```c++
#include "utils/preallocated_multiset.hpp"

#include <algorithm>
```

__Source code:__ 

```c++
void main(auto const&, auto const&) {}
```

## Tags

__Id:__ f5898fc46c3d7f70c7c8489a40b84b0d

__Category:__ Filters

__Release state:__ Stable