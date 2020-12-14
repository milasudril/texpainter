# DFT Backward

This image computes the DFT of a grayscale image.

## Input ports

__Input:__ (Image spectrum) The image spectrum (re)construct the image from

## Output ports

__Output:__ (Grayscale image) The result. Notice that it is possible that the output contains out-of-range values.

## Implementation

__Includes:__ 

```c++
#include "dft/engine.hpp"
#include "pixel_store/image.hpp"
```

__Source code:__ 

```c++
using TempBuffer = Texpainter::PixelStore::BasicImage<ComplexValue>;
namespace Dft    = Texpainter::Dft;

void main(auto const& args)
{
	auto const size = args.size();
	TempBuffer output_buffer{size};
	auto& engine = Dft::engineInstance();
	engine.run<Dft::Direction::Backward>(size, input<0>(args), output_buffer.pixels().data());

	auto sign_row = 1;
	for(uint32_t row = 0; row < size.height(); ++row)
	{
		auto sign_col = 1;
		for(uint32_t col = 0; col < size.width(); ++col)
		{
			output<0>(args, col, row) = output_buffer(col, row).real() * sign_col * sign_row;
			sign_col *= -1;
		}
		sign_row *= -1;
	}
}
```

## Tags

__Id:__ E4CA00ACA31445D47FB510B3102C9529

__Category:__ Converters