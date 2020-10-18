//@	{"targets":[{"name":"document.hpp", "type":"include"}]}

#ifndef TEXPAINTER_MODEL_DOCUMENT_HPP
#define TEXPAINTER_MODEL_DOCUMENT_HPP

#include "./palette_index.hpp"
#include "./layer.hpp"
#include "./layer_index.hpp"
#include "./brush.hpp"
#include "./item_name.hpp"

#include "utils/sorted_sequence.hpp"
#include "utils/mutator.hpp"

#include <string>
#include <cassert>

namespace Texpainter::Model
{
	class Document
	{
	public:
		using LayerStack = SortedSequence<ItemName, Layer, LayerIndex>;

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

		auto const& currentLayer() const { return m_current_layer; }

		Document& currentLayer(ItemName&& current_layer)
		{
			assert(m_layers[current_layer] != nullptr);
			m_current_layer = std::move(current_layer);
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

		ItemName m_current_layer;
		ItemName m_current_palette;
		BrushInfo m_current_brush;

		bool m_dirty;
	};

	inline Layer const* currentLayer(Document const& doc)
	{
		auto const& layers = doc.layers();
		return layers[doc.currentLayer()];
	}


	inline auto visibleLayersByIndex(Document const& doc)
	{
		auto layers = doc.layersByIndex();
		std::vector<std::reference_wrapper<Layer const>> ret;
		ret.reserve(doc.layers().size().value());
		if(std::ranges::any_of(layers, [](auto const& layer) { return layer.isolated(); }))
		{
			std::ranges::copy_if(layers, std::back_inserter(ret), [](auto const& layer) {
				return layer.visible() && layer.isolated();
			});
			return ret;
		}

		std::ranges::copy_if(
		    layers, std::back_inserter(ret), [](auto const& layer) { return layer.visible(); });
		return ret;
	}


}

#endif
