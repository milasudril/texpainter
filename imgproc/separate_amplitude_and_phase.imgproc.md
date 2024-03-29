# Separate amplitude and phase

This image processor takes an image spectrum, and converts it to two grayscale images, one representing the amplitude, and the other one representing the phase.

## Input ports

__Input:__ (Image spectrum) The image spectrum to convert

## Output ports

__Amplitude:__ (Grayscale image) The amplitude (absolute value) of the spectrum

__Phase:__ (Grayscale image) The phase (argument) of the spectrum. The phase is computed by using `std::arg`.

## Implementation

__Includes:__ 

```c++
#include <algorithm>
```

__Source code:__ 

```c++
void main(auto const& arg)
{
	auto size = area(arg.canvasSize());
	std::transform(input<0>(arg), input<0>(arg) + size, output<0>(arg), [](auto val) {
		return std::abs(val);
	});
	std::transform(input<0>(arg), input<0>(arg) + size, output<1>(arg), [](auto val) {
		return std::arg(val);
	});
}
```

## Tags

__Id:__ d2763711c660c5f8a7bd761276cc6757

__Category:__ Converters

__Release state:__ Stable