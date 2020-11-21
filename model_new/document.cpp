//@	{
//@	 "targets":[{"name":"document.o", "type":"object"}]
//@	}

#include "./document.hpp"

#include <algorithm>

bool Texpainter::Model::Document::dirty() const
{
	return m_dirty /* || m_compositor.dirty() */
	       || std::ranges::any_of(m_images, [](auto const& item) { return item.second.dirty(); })
	       || std::ranges::any_of(m_palettes, [](auto const& item) { return item.second.dirty(); });
}