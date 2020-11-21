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
