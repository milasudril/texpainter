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

Texpainter::PixelStore::Image Texpainter::Model::render(Document const& document)
{
	PixelStore::Image ret{document.canvasSize()};
	std::ranges::for_each(document.images(), [](auto& item) {
		item.second.processor.get().processor().source(item.second.source.get().pixels());
	});

	std::ranges::for_each(document.palettes(), [](auto& item) {
		item.second.processor.get().processor().source(item.second.source.get());
	});

	document.compositor().process(ret.pixels());
	return ret;
}