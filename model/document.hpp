//@	{"targets":[{"name":"document.hpp", "type":"include"}]}

#ifndef TEXPAINTER_DOCUMENT_HPP
#define TEXPAINTER_DOCUMENT_HPP

#include "./image.hpp"
#include "./palette.hpp"

namespace Texpainter::Model
{
	class Document
	{
	public:
		Document(): m_palette{20}, m_sel_color{0}, m_image{512, 512} {}

		explicit Document(Palette&& pal, Image&& img, size_t sel_color = 0)
		    : m_palette{std::move(pal)}
		    , m_sel_color{sel_color}
		    , m_image{std::move(img)}
		{
			m_sel_color = m_sel_color > std::size(m_palette) ? 0 : m_sel_color;
		}

		Image const& image() const { return m_image; }

		Image& image() { return m_image; }

		Palette const& palette() const { return m_palette; }

		Palette& palette() { return m_palette; }

		size_t selectedColorIndex() const { return m_sel_color; }

		Document& selectedColorIndex(size_t index)
		{
			m_sel_color = index;
			return *this;
		}

	private:
		Palette m_palette;
		size_t m_sel_color;
		Image m_image;
	};

	template<class OutputStream>
	void write(Document const& doc, OutputStream stream)
	{
		write(doc.palette(), stream);
		write(static_cast<uint32_t>(doc.selectedColorIndex()), stream);
		write(doc.image(), stream);
	}

	template<class InputStream>
	Document read(Empty<Document>, InputStream stream)
	{
		auto pal       = read(Empty<Palette>{}, stream);
		auto sel_color = read(Empty<uint32_t>{}, stream);
		auto image     = read(Empty<Image>{}, stream);

		return Document{std::move(pal), std::move(image), sel_color};
	}

	inline auto selectedColor(Document const& doc)
	{
		return doc.palette()[doc.selectedColorIndex()];
	}
}

#endif