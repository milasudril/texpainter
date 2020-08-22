//@	{"targets":[{"name":"document.hpp", "type":"include"}]}

#ifndef TEXPAINTER_MODEL_DOCUMENT_HPP
#define TEXPAINTER_MODEL_DOCUMENT_HPP

#include "./palette_index.hpp"
#include "./layer.hpp"
#include "./layer_index.hpp"
#include "./brush.hpp"

#include "pixel_store/palette.hpp"
#include "pixel_store/color_index.hpp"
#include "utils/sorted_sequence.hpp"
#include "utils/mutator.hpp"

#include <string>
#include <cassert>

namespace Texpainter::Model
{
	class Document
	{
	public:
		using LayerStack        = SortedSequence<std::string, Layer, LayerIndex>;
		using PaletteCollection = SortedSequence<std::string, PixelStore::Palette, PaletteIndex>;

		explicit Document(Size2d canvas_size)
		    : m_canvas_size{canvas_size}
		    , m_current_brush{1.0f / 64.0f, BrushType::Circle}
		    , m_dirty{false}
		{
		}

		bool dirty() const { return m_dirty; }

		Size2d canvasSize() const { return m_canvas_size; }

		Document& canvasSize(Size2d size)
		{
			m_canvas_size = size;
			m_dirty       = true;
			return *this;
		}


		LayerStack const& layers() const { return m_layers; }

		auto layersByIndex() const { return m_layers.valuesByIndex(); }

		Document& layers(LayerStack&& layers_new)
		{
			m_layers = std::move(layers_new);
			m_dirty  = true;
			return *this;
		}

		template<Mutator<LayerStack> Func>
		bool layersModify(Func&& f)
		{
			m_dirty = f(m_layers) || m_dirty;
			return m_dirty;
		}


		std::string const& currentLayer() const { return m_current_layer; }

		Document& currentLayer(std::string&& current_layer)
		{
			assert(m_layers[current_layer] != nullptr);
			m_current_layer = std::move(current_layer);
			m_dirty         = true;
			return *this;
		}


		PaletteCollection const& palettes() const { return m_palettes; }

		Document& palettes(PaletteCollection&& palettes_new)
		{
			m_palettes = std::move(palettes_new);
			m_dirty    = true;
			return *this;
		}

		template<Mutator<PaletteCollection> Func>
		bool palettesModify(Func&& f)
		{
			m_dirty = f(m_palettes) || m_dirty;
			return m_dirty;
		}


		std::string const& currentPalette() const { return m_current_palette; }

		Document& currentPalette(std::string&& current_palette)
		{
			m_current_palette = std::move(current_palette);
			m_current_color   = PixelStore::ColorIndex{0};
			m_dirty           = true;
			return *this;
		}


		PixelStore::ColorIndex currentColor() const { return m_current_color; }

		Document& currentColor(PixelStore::ColorIndex i)
		{
			m_current_color = i;
			m_dirty         = true;
			return *this;
		}

		BrushInfo currentBrush() const { return m_current_brush; }

		Document& currentBrush(BrushInfo brush)
		{
			m_current_brush = brush;
			m_dirty         = true;
			return *this;
		}

	private:
		Size2d m_canvas_size;
		LayerStack m_layers;
		PaletteCollection m_palettes;
		PixelStore::ColorIndex m_current_color;

		std::string m_current_layer;
		std::string m_current_palette;
		BrushInfo m_current_brush;

		bool m_dirty;
	};

	inline PixelStore::Palette const* currentPalette(Document const& doc)
	{
		auto const& palettes = doc.palettes();
		return palettes[doc.currentPalette()];
	}

	inline PixelStore::Pixel currentColor(Document const& doc)
	{
		auto i                       = currentPalette(doc);
		constexpr auto default_color = PixelStore::Pixel{1.0f / 3, 1.0f / 3, 1.0f / 3, 1.0f};
		if(i == nullptr) { return default_color; }

		return doc.currentColor() <= i->lastIndex() ? (*i)[doc.currentColor()] : default_color;
	}

	inline bool colorIndexValid(Document const& doc, PixelStore::ColorIndex index)
	{
		auto i = currentPalette(doc);
		return (i == nullptr) ? false : (index.value() < i->size()) ? true : false;
	}

	inline Layer const* currentLayer(Document const& doc)
	{
		auto const& layers = doc.layers();
		return layers[doc.currentLayer()];
	}

}

#endif
