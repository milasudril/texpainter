//@	{
//@	 "targets":[{"name":"document.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"document.o", "rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_MODELNEW_DOCUMENT_HPP
#define TEXPAINTER_MODELNEW_DOCUMENT_HPP

#include "./compositor.hpp"
#include "./item_name.hpp"
#include "./palette.hpp"

#include "pixel_store/image.hpp"
#include "utils/with_status.hpp"

namespace Texpainter::Model
{
	class Document
	{
	public:
		explicit Document(Size2d canvas_size): m_canvas_size{canvas_size}, m_dirty{false} {}

		bool dirty() const;

		Size2d canvasSize() const { return m_canvas_size; }

		Document& canvasSize(Size2d size)
		{
			m_canvas_size = size;
			m_dirty       = true;
			return *this;
		}

		Compositor const& compositor() const { return m_compositor.get(); }

		template<InplaceMutator<Compositor> Mutator>
		bool modifyCompositor(Mutator&& mut)
		{
			return m_compositor.modify(std::forward<Mutator>(mut));
		}

		auto const& images() const { return m_images; }

		PixelStore::Image const* image(ItemName const& item) const
		{
			auto i = m_images.find(item);
			return i != std::end(m_images) ? &i->second.get() : nullptr;
		}

		template<InplaceMutator<Compositor> Mutator>
		bool modifyImage(Mutator&& mut, ItemName const& item)
		{
			auto i = m_images.find(item);
			if(i == std::end(m_images)) [[unlikely]]
				{
					return false;
				}

			return i->second.modify(std::forward<Mutator>(mut));
		}

		auto const& palettes() const { return m_palettes; }

		Palette const* palette(ItemName const& item) const
		{
			auto i = m_palettes.find(item);
			return i != std::end(m_palettes) ? &i->second.get() : nullptr;
		}

		template<InplaceMutator<Compositor> Mutator>
		bool modifyPalette(Mutator&& mut, ItemName const& item)
		{
			auto i = m_palettes.find(item);
			if(i == std::end(m_palettes)) [[unlikely]]
				{
					return false;
				}

			return i->second.modify(std::forward<Mutator>(mut));
		}

	private:
		Size2d m_canvas_size;
		WithStatus<Compositor> m_compositor;
		std::map<ItemName, WithStatus<PixelStore::Image>> m_images;
		std::map<ItemName, WithStatus<Palette>> m_palettes;
		bool m_dirty;
	};

	inline PixelStore::Image render(Document const& document)
	{
		PixelStore::Image ret{document.canvasSize()};
		document.compositor().process(ret.pixels());
		return ret;
	}
}

#endif
