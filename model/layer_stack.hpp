//@	{"targets":[{"name":"layer_stack.hpp", "type":"include"}]}

#ifndef TEXPAINTER_LAYERSTACK_HPP
#define TEXPAINTER_LAYERSTACK_HPP

#include "./layer.hpp"
#include "./layer_index.hpp"

#include "utils/sequence.hpp"

#include <vector>
#include <cassert>

namespace Texpainter::Model
{
	using LayerStack = Sequence<Layer, LayerIndex>;

	void render(LayerStack const& layers, Span2d<Pixel> ret)
	{
		std::ranges::for_each(layers, [ret](auto const& layer) {
			if(layer.visible()) { render(layer, ret); }
		});
	}
}

#endif