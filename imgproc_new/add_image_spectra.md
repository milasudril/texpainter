# Add image spectra

This image processor takes two image spectra `A` and `B` and computes their sum. The output `Sum` is
a new spectrum that is the sum of `A` and `B`.

## Input ports

__A:__ (Grayscale complex pixels) First operand

__B:__ (Grayscale complex pixels) Second operand

## Output ports

__Sum:__ (Grayscale complex pixels) The sum of `A` and `B`

## Implementation

The implementation of this image processor is trivial. Simply call `std::transform`, with the
`std::plus` function object, as defined in the C++ standard library.

__Includes:__

```
#include <algorithm>
#include <functional>
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
                   std::plus{});
}
```

## Tags

__Id:__ fb6eec86decb18e0f52f01a20eaa6653

__Category:__ Arithmetic operators