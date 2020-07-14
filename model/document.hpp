//@	{"targets":[{"name":"document.hpp", "type":"include"}]}

#ifndef TEXPAINTER_DOCUMENT_HPP
#define TEXPAINTER_DOCUMENT_HPP

#include "./image.hpp"
#include "./palette.hpp"
#include "./layer_stack.hpp"

namespace Texpainter::Model
{
	class Document
	{
	public:

	private:
		Size2d m_canvas_size;
		std::vector<Palette> m_palettes;
		uint32_t m_sel_palette;
		uint32_t m_sel_color;
		LayerStack m_layers;
	};
}

#endif