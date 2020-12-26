# Apply colormap

This image processor applies a colormap to a grayscale image. The colormap is taken from a palette. Intensity values are mapped such that lower values are mapped to a lower index, and higher values are mapped to a higher index. Black is mapped to index 0, and white is mapped to index 15.

## Input ports

__Image:__ (Grayscale image) The image that the colormap should be applied to

__Palette:__ (Palette) The palette that should be used for the mapping

## Output ports

__Output:__ (RGBA image) The resulting image

## Parameters

The parameters to this image processor are the weights to apply to each color index. A higher weight will result in a color appearing more frequently, in case the input image is white noise. The weights will automatically be normalized so that their total sum is equal to 1.0.

__0:__ (= 1.0)

__1:__ (= 1.0)

__2:__ (= 1.0)

__3:__ (= 1.0)

__4:__ (= 1.0)

__5:__ (= 1.0)

__6:__ (= 1.0)

__7:__ (= 1.0)

__8:__ (= 1.0)

__9:__ (= 1.0)

__A:__ (= 1.0)

__B:__ (= 1.0)

__C:__ (= 1.0)

__D:__ (= 1.0)

__E:__ (= 1.0)

__F:__ (= 1.0)

## Implementation

__Includes:__ 

```c++
#include "utils/discrete_pdf.hpp"
```

__Source code:__ 

```c++
void main(auto const& args, auto const& params)
{
	auto const size = args.size().area();
	std::array<double, Palette::size()> vals;
	std::ranges::transform(params.values(), std::begin(vals), [](auto val) { return val.value(); });

	std::transform(input<0>(args),
	               input<0>(args) + size,
	               output<0>(args),
	               [&palette = input<1>(args).get(),
	                pdf      = Texpainter::DiscretePdf<double, Palette::size()>{vals}](auto val) {
		               using IntegerType = Palette::index_type::element_type;
		               auto index        = static_cast<IntegerType>(pdf.eventIndex(val));
		               return palette[Palette::index_type{index}];
	               });
}
```

## Tags

__Id:__ c3847088432d8e2ead6e112d57a0ca22

__Category:__ Converters