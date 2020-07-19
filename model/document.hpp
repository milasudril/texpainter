//@	{"targets":[{"name":"document.hpp", "type":"include"}]}

#ifndef TEXPAINTER_DOCUMENT_HPP
#define TEXPAINTER_DOCUMENT_HPP

#include "./utils/double_key_map.hpp"
#include "./color_index.hpp"
#include "./palette_index.hpp"

#include "utils/mutator.hpp"

#include <string>
#include <cassert>

namespace Texpainter::Model
{
	class Document
	{
	public:
		using LayerStack = DoubleKeyMap<Layer, std::string, LayerIndex>;
		using PaletteCollection = DoubleKeyMap<Palette, std::string, PaletteIndex>;

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

		template<Mutator<LayerStack> Func>
		Document& layersModify(Func&& f)
		{
			m_dirty = f(m_layers);
			return *this;
		}


		std::string const& currentLayer() const
		{
			return m_current_layer;
		}

		Document& currentLayer(std::string&& current_layer)
		{
			assert(m_layers[current_layer] != nullptr);
			m_current_layer = std::move(current_layer);
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

		template<Mutator<PaletteCollection> Func>
		Document& palettesModify(Func&& f)
		{
			m_dirty = f(m_palettes);
			return *this;
		}


		std::string const& currentPalette() const
		{
			return m_current_palette;
		}

		Document& currentPalette(std::string&& current_palette)
		{
			assert(m_palettes[current_palette] != nullptr);
			m_current_palette = std::move(current_palette);
			m_dirty = true;
			return *this;
		}


		ColorIndex currentColor() const
		{
			return m_current_color;
		}

		Document& currentColor(ColorIndex i)
		{
			assert(i <= m_palettes[m_current_palette]->lastIndex());
			m_current_color = i;
			m_dirty = true;
			return *this;
		}

	private:
		Size2d m_canvas_size;
		LayerStack m_layers;
		PaletteCollection m_palettes;
		ColorIndex m_current_color;

		std::string m_current_layer;
		std::string m_current_palette;

		bool m_dirty;
	};
}

#endif