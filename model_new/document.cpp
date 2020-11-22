//@	{
//@	 "targets":[{"name":"document.o", "type":"object"}]
//@	}

#include "./document.hpp"

#include <algorithm>

bool Texpainter::Model::Document::dirty() const
{
	return m_dirty /* || m_compositor.dirty() */
	       || std::ranges::any_of(images(),
	                              [](auto const& item) { return item.second.source.dirty(); })
	       || std::ranges::any_of(palettes(),
	                              [](auto const& item) { return item.second.source.dirty(); });
}