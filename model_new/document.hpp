//@	{
//@	 "targets":[{"name":"document.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"document.o", "rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_MODELNEW_DOCUMENT_HPP
#define TEXPAINTER_MODELNEW_DOCUMENT_HPP

#include "./compositor.hpp"
#include "./compositor_input.hpp"
#include "./item_name.hpp"
#include "./image_source.hpp"
#include "./palette_source.hpp"

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

		explicit Document(Size2d canvas_size): Size2d{canvas_size}, m_dirty{false} {}

		bool dirty() const;

		Size2d canvasSize() const { return static_cast<Size2d>(*this); }

		Document& canvasSize(Size2d size)
		{
			static_cast<Size2d&>(*this) = size;
			m_dirty                     = true;
			return *this;
		}

		Compositor const& compositor() const { return static_cast<Compositor const&>(*this); }

		Compositor& compositor() { return static_cast<Compositor&>(*this); }


		auto const& images() const { return get(std::type_identity<PixelStore::Image>{}); }

		auto image(ItemName const& name) const
		{
			return get(std::type_identity<PixelStore::Image>{}, name);
		}

		auto insert(ItemName const& name, PixelStore::Image&& img)
		{
			return insert(name, std::forward<PixelStore::Image>(img), *this, m_input_nodes);
		}

		auto eraseImage(ItemName const& name)
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

		auto erasePalette(ItemName const& name)
		{
			return erase(std::type_identity<Palette>{}, name, *this, m_input_nodes);
		}


		Compositor::NodeItem const* inputNodeItem(ItemName const& name) const
		{
			auto i = m_input_nodes.find(name);
			return i != std::end(m_input_nodes) ? &i->second : nullptr;
		}


	private:
		std::map<ItemName, Compositor::NodeItem> m_input_nodes;
		bool m_dirty;
	};

	PixelStore::Image render(Document const& document);
}

#endif
