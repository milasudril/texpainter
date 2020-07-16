//@	{"targets":[{"name":"document.hpp", "type":"include"}]}

#ifndef TEXPAINTER_DOCUMENT_HPP
#define TEXPAINTER_DOCUMENT_HPP

#include "./layer_stack.hpp"
#include "./color_index.hpp"
#include "./palette_collection.hpp"

#include <cassert>

namespace Texpainter::Model
{
	class Document
	{
	public:
		explicit Document(Size2d canvas_size): m_canvas_size{canvas_size}, m_dirty{false}
		{
		}

		bool dirty() const
		{
			return m_dirty;
		}

		Size2d canvasSize() const
		{
			return m_canvas_size;
		}

		Document& canvasSize(Size2d size)
		{
			m_canvas_size = size;
			m_dirty = true;
			return *this;
		}


		LayerStack const& layers() const
		{
			return m_layers;
		}

		Document& layers(LayerStack&& layers_new)
		{
			m_layers = std::move(layers_new);
			m_dirty = true;
			return *this;
		}

		template<class Func>
		Document& layersModify(Func&& f)
		{
			f(m_layers);
			m_dirty = true;
			return *this;
		}


		LayerIndex currentLayer() const
		{
			return m_current_layer;
		}

		Document& currentLayer(LayerIndex i)
		{
			assert(i <= m_layers.lastIndex());
			m_dirty = true;
			return *this;
		}


		PaletteCollection const& palettes() const
		{
			return m_palettes;
		}

		Document& palettes(PaletteCollection&& palettes_new)
		{
			m_palettes = std::move(palettes_new);
			m_dirty = true;
			return *this;
		}

		template<class F>
		Document& palettesModify(F&& f)
		{
			f(m_palettes);
			m_dirty = true;
			return *this;
		}


		PaletteIndex currentPalette() const
		{
			return m_current_palette;
		}

		Document& currentPalette(PaletteIndex i)
		{
			assert(i <= m_palettes.lastIndex());
			m_current_palette = i;
			m_current_color = std::min(m_palettes[m_current_palette].lastIndex(), m_current_color);
			m_dirty = true;
			return *this;
		}


		ColorIndex currentColor() const
		{
			return m_current_color;
		}

		Document& currentColor(ColorIndex i)
		{
			assert(i <= m_palettes[m_current_palette].lastIndex());
			m_current_color = i;
			m_dirty = true;
			return *this;
		}


	private:
		Size2d m_canvas_size;
		LayerStack m_layers;
		LayerIndex m_current_layer;

		PaletteCollection m_palettes;
		PaletteIndex m_current_palette;
		ColorIndex m_current_color;

		//		std::vector<std::string> m_palette_names;
		//		std::vector<std::string> m_layer_names;

		bool m_dirty;
	};
}

#endif