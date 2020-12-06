# Add image spectra

This image processor takes two image spectra `A` and `B` and computes their sum. The output `Sum` is
a new spectrum that is the sum of `A` and `B`.

## Input ports

__A__: (Grayscale complex pixels) First operand

__B__: (Grayscale complex pixels) Second operand

## Output ports

__Sum__: (Grayscale complex pixels) The sum of `A` and `B`

## Implementation

__Includes:__

```
#include <algorithm>
#include <functional>
```

__Source code:__

```
void main(auto const& args)
{
	auto const size = args.size().area();
	std::transform(input<0>(args),
	               input<0>(args) + size,
	               input<1>(args),
	               output<0>(args),
	               std::multiplies{});
}
```