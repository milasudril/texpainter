//@	{"targets":[{"name":"palette.hpp", "type":"include"}]}

#ifndef TEXPAINTER_MODELNEW_PALETTE_HPP
#define TEXPAINTER_MODELNEW_PALETTE_HPP

#include "pixel_store/palette.hpp"

namespace Texpainter::Model
{
	using Palette = PixelStore::Palette<16>;
}

#endif
