//@	{"targets":[{"name":"document.hpp", "type":"include"}]}

#ifndef TEXPAINTER_MODEL_DOCUMENT_HPP
#define TEXPAINTER_MODEL_DOCUMENT_HPP

#include "./compositor.hpp"
#include "./item_name.hpp"

#include "pixel_store/image.hpp"
#include "utils/with_status.hpp"

namespace Texpainter::Model
{
	class Document
	{
	public:
		explicit Document(Size2d canvas_size): m_canvas_size{canvas_size}, m_dirty{false} {}

		bool dirty() const { return m_dirty; }

		Size2d canvasSize() const { return m_canvas_size; }

		Document& canvasSize(Size2d size)
		{
			m_canvas_size = size;
			m_dirty       = true;
			return *this;
		}

		Compositor const& compositor() const { return m_compositior.content(); }

	private:
		Size2d m_canvas_size;
		WithStatus<Compositor> m_compositior;
		std::map<ItemName, WithStatus<PixelStore::Image>>;
		std::map<ItemName, WithStatus<PixelStore::Palette<16>>>;
		bool m_dirty;
	};

	inline PixelStore::Image render(Document const& document)
	{
		PixelStore::Image ret{document.canvasSize()};
		document.content().process(ret.pixels());
		return ret;
	}
}

#endif
