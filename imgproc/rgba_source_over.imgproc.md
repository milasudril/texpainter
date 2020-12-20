# RGBA Source over

This image processor implements the alpha over operator. That is

$$\text{Output} = \text{Source} + \text{Destination}\times\left(1 - \text{Source}_\alpha \right)$$

## Input ports

__Source:__ (RGBA image) The source image

__Destination:__ (RGBA image) The destination image

## Output ports

__Output:__ (RGBA image) The composed image

## Implementation

__Includes:__ 

```c++
#include <algorithm>
```

__Source code:__ 

```c++
void main(auto const& args)
{
	auto const size = args.size().area();
	std::transform(input<0>(args),
	               input<0>(args) + size,
	               input<1>(args),
	               output<0>(args),
	               [](auto src, auto dest) { return src + dest * (1.0f - src.alpha()); });
}
```

## Tags

__Id:__ b65334b771e511c1e4cc198d60ef792a

__Category:__ Arithmetic operators