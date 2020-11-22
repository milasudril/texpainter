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

		class CompositorProxy
		{
		public:
			explicit CompositorProxy(Document& owner, Compositor& comp)
			    : m_owner{owner}
			    , m_compositor{comp}
			{
			}

			CompositorProxy& connect(NodeId a, InputPortIndex sink, NodeId b, OutputPortIndex src)
			{
				m_compositor.get().connect(a, sink, b, src);
				return *this;
			}

			void clearValidationState() { m_compositor.get().clearValidationState(); }

			auto nodesWithId() { return m_compositor.get().nodesWithId(); }

			auto erase(Compositor::NodeId id)
			{
				auto name = m_owner.get().inputNodeName(id);
				if(name == nullptr) [[unlikely]]
					{
						m_compositor.get().erase(id);
						return true;
					}

				if(!m_owner.get().eraseImage(*name)) [[unlikely]]
					{
						return m_owner.get().erasePalette(*name);
					}

				return true;
			}

		private:
			std::reference_wrapper<Document> m_owner;
			std::reference_wrapper<Compositor> m_compositor;
		};

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

		CompositorProxy compositor()
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


	private:
		std::map<ItemName, Compositor::NodeItem> m_input_nodes;
		bool m_dirty;
	};

	PixelStore::Image render(Document const& document);
}

#endif
