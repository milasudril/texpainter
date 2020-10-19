//@	{
//@	 "targets":[{"name":"palette_generate.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"palette_generate.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_MODEL_PALETTEGENERATE_HPP
#define TEXPAINTER_MODEL_PALETTEGENERATE_HPP

#include "pixel_store/palette.hpp"

namespace Texpainter::Model
{
	PixelStore::Palette<16> generatePalette(std::array<PixelStore::Pixel, 4> const& base_colors);
}

#endif