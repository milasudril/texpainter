//@	{
//@	 "targets":[{"name":"palette_generate.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"palette_generate.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_MODEL_PALETTEGENERATE_HPP
#define TEXPAINTER_MODEL_PALETTEGENERATE_HPP

#include "pixel_store/palette.hpp"

#include <algorithm>

namespace Texpainter::Model
{
	PixelStore::Palette<16> generatePaletteByHue(
	    std::array<PixelStore::Pixel, 4> const& base_colors);

	PixelStore::Palette<16> generatePaletteByIntensity(
	    std::array<PixelStore::Pixel, 4> const& base_colors);

	inline auto generatePalette(std::array<PixelStore::Pixel, 4> const& base_colors,
	                            bool by_intensity,
	                            bool reversed)
	{
		auto ret = by_intensity ? generatePaletteByIntensity(base_colors)
		                        : generatePaletteByHue(base_colors);

		if(reversed) { std::ranges::reverse(ret); }

		return ret;
	}
}

#endif