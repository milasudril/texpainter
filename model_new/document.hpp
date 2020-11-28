//@	{
//@	 "targets":[{"name":"document.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"document.o", "rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_MODELNEW_DOCUMENT_HPP
#define TEXPAINTER_MODELNEW_DOCUMENT_HPP

#include "./compositor_proxy.hpp"
#include "./compositor_input.hpp"
#include "./item_name.hpp"
#include "./image_source.hpp"
#include "./palette_source.hpp"
#include "./compositor_proxy.hpp"
#include "./brush.hpp"

#include "pixel_store/image.hpp"
#include "utils/with_status.hpp"

namespace Texpainter::Model
{
	namespace detail
	{
		template<>
		struct ImageProcessor<WithStatus<PixelStore::Image>>
		{
			using type = ImageSource;
		};

		template<>
		struct ImageProcessor<WithStatus<Palette>>
		{
			using type = PaletteSource;
		};
	}

	class Document: private Size2d,
	                private Compositor,
	                private CompositorInputManager<PixelStore::Image>,
	                private CompositorInputManager<Palette>
	{
		using CompositorInputManager<PixelStore::Image>::insert;
		using CompositorInputManager<Palette>::insert;
		using CompositorInputManager<PixelStore::Image>::erase;
		using CompositorInputManager<Palette>::erase;


	public:
		using CompositorInputManager<PixelStore::Image>::get;
		using CompositorInputManager<PixelStore::Image>::modify;
		using CompositorInputManager<Palette>::get;
		using CompositorInputManager<Palette>::modify;

		explicit Document(Size2d canvas_size)
		    : Size2d{canvas_size}
		    , m_current_brush{BrushInfo{0.5f, BrushShape::Circle}}
		{
		}

		Size2d canvasSize() const { return static_cast<Size2d>(*this); }

		Document& canvasSize(Size2d size)
		{
			static_cast<Size2d&>(*this) = size;
			return *this;
		}

		Compositor const& compositor() const { return static_cast<Compositor const&>(*this); }

		CompositorProxy<Document> compositor()
		{
			return CompositorProxy{*this, static_cast<Compositor&>(*this)};
		}


		auto const& images() const { return get(std::type_identity<PixelStore::Image>{}); }

		auto image(ItemName const& name) const
		{
			return get(std::type_identity<PixelStore::Image>{}, name);
		}

		auto insert(ItemName const& name, PixelStore::Image&& img)
		{
			return insert(name, std::forward<PixelStore::Image>(img), *this, m_input_nodes);
		}

		bool eraseImage(ItemName const& name)
		{
			return erase(std::type_identity<PixelStore::Image>{}, name, *this, m_input_nodes);
		}


		auto const& palettes() const { return get(std::type_identity<Palette>{}); }

		auto palette(ItemName const& name) const
		{
			return get(std::type_identity<Palette>{}, name);
		}

		auto insert(ItemName const& name, Palette&& pal)
		{
			return insert(name, std::forward<Palette>(pal), *this, m_input_nodes);
		}

		bool erasePalette(ItemName const& name)
		{
			return erase(std::type_identity<Palette>{}, name, *this, m_input_nodes);
		}


		Compositor::NodeItem const* inputNodeItem(ItemName const& name) const
		{
			auto i = m_input_nodes.find(name);
			return i != std::end(m_input_nodes) ? &i->second : nullptr;
		}

		ItemName const* inputNodeName(Compositor::NodeId id) const
		{
			auto i = std::ranges::find_if(
			    m_input_nodes, [id](auto const& item) { return item.second.first == id; });
			return i != std::end(m_input_nodes) ? &i->first : nullptr;
		}

		auto erase(Compositor::NodeId id)
		{
			auto name = inputNodeName(id);
			if(name == nullptr) [[likely]]
				{
					Compositor::erase(id);
					return true;
				}

			if(!eraseImage(*name)) { return erasePalette(*name); }

			return true;
		}

		auto copy(Compositor::NodeId id, ItemName const& name_new)
		{
			auto name = inputNodeName(id);
			assert(name != nullptr);

			if(auto img = image(*name); img != nullptr)
			{
				insert(name_new, Texpainter::PixelStore::Image{img->source.get()});
				return inputNodeItem(name_new);
			}

			if(auto pal = palette(*name); pal != nullptr)
			{
				insert(name_new, Texpainter::PixelStore::Palette{pal->source.get()});
				return inputNodeItem(name_new);
			}
			__builtin_unreachable();
		}

		auto const& nodeLocations() const { return m_node_locations; }

		Document& nodeLocations(std::map<FilterGraph::NodeId, vec2_t>&& locs)
		{
			m_node_locations = std::move(locs);
			return *this;
		}

		Document& currentBrush(BrushInfo brush)
		{
			m_current_brush = brush;
			return *this;
		}

		BrushInfo currentBrush() const { return m_current_brush; }


	private:
		std::map<ItemName, Compositor::NodeItem> m_input_nodes;
		std::map<FilterGraph::NodeId, vec2_t> m_node_locations;
		BrushInfo m_current_brush;
	};

	PixelStore::Image render(Document const& document);
}

#endif
