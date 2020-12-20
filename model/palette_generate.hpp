//@	{
//@	 "targets":[{"name":"palette_generate.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"palette_generate.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_MODEL_PALETTEGENERATE_HPP
#define TEXPAINTER_MODEL_PALETTEGENERATE_HPP

#include "./palette.hpp"

#include <algorithm>

namespace Texpainter::Model
{
	struct PaletteParameters
	{
		std::array<PixelStore::Pixel, 4> colors;
		bool intensity_to_intensity;
		bool intensity_to_alpha;
		bool by_intensity;
		bool reversed;
	};

	Palette generatePalette(PaletteParameters const& params);
}

#endif