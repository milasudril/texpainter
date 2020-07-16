//@	{"targets":[{"name":"palette_collection.hpp", "type":"include"}]}

#ifndef TEXPAINTER_PALETTECOLLECTION_HPP
#define TEXPAINTER_PALETTECOLLECTION_HPP

#include "./palette.hpp"
#include "./palette_index.hpp"

#include "utils/sequence.hpp"

#include <vector>
#include <cassert>

namespace Texpainter::Model
{
	using PaletteCollection = Sequence<Palette, PaletteIndex>;
}

#endif