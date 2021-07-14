# Amplitude and phase to image spectrum

This image processor takes two grayscale images, and combines them into an image spectrum. The first input is the amplitude, and the second input is the phase.

## Input ports

__Amplitude:__ (Grayscale image) The amplitude (absolute value) of the spectrum

__Phase:__ (Grayscale image) The phase (argument) of the spectrum.

## Output ports

__Result:__ (Image spectrum) The resulting image spectrum

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
	std::transform(input<0>(arg), input<0>(arg) + size, input<1>(arg), output<0>(arg), std::polar<double>);
}
```

## Tags

__Id:__ 43cf35b96593174959fd964da3beaf76

__Category:__ Converters

__Release state:__ Stable