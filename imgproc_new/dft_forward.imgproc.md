# DFT Forward

This image computes the DFT of a grayscale image.

## Input ports

__Input:__ (Grayscale image) The input image

## Output ports

__Output:__ (Image spectrum) The computed image spectrum

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
	auto sign_row   = 1;
	auto const size = args.size();
	TempBuffer input_buffer{size};
	for(uint32_t row = 0; row < size.height(); ++row)
	{
		auto sign_col = 1;
		for(uint32_t col = 0; col < size.width(); ++col)
		{
			input_buffer(col, row) = ComplexValue{input<0>(args, col, row) * sign_row * sign_col};
			sign_col *= -1;
		}
		sign_row *= -1;
	}

	auto const out = output<0>(args);
	auto& engine   = Dft::engineInstance();
	engine.run<Dft::Direction::Forward>(size, input_buffer.pixels().data(), out);
	auto const A = size.area();
	std::for_each(out, out + A, [A](auto& val) { return val /= A; });
}
```

## Tags

__Id:__ 3B0189D5F690713FF7D02B05E74571D1

__Category:__ Converters