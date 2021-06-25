# Gradient size from normal vector

This image processor takes the normal vector (possibly computed by \ref{Make topographic map}), and computes a corresponding gradient vector. Then it computes the size of the gradient vector.

## Input ports

__Input:__ (Topography data)

## Output ports

__Output:__ (Grayscale image) The gradient size

## Implementation

__Includes:__ 

```c++
#include <numbers>
```

__Source code:__ 

```c++
using Texpainter::vec4_t;

void main(auto const& arg)
{
	std::transform(
	    input<0>(arg), input<0>(arg) + area(arg.canvasSize()), output<0>(arg), [](auto val) {
		    auto const n = val.normal();
		    return Texpainter::length(vec2_t{n[0], n[1]});
	    });
}
```

## Tags

__Id:__ 88d4479efdb604f0a70d713c0d2f0a22

__Category:__ Converters

__Release state:__ Stable