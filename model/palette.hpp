//@	{"targets":[{"name":"palette.hpp", "type":"include"}]}

#ifndef TEXPAINTER_MODEL_PALETTE_HPP
#define TEXPAINTER_MODEL_PALETTE_HPP

#include "pixel_store/palette.hpp"

namespace Texpainter::Model
{
	using Palette = PixelStore::RgbaPalette<16>;
}

#endif
