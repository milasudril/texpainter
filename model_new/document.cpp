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
	std::ranges::for_each(document.images(), [&document](auto& item) {
		item.second.processor.get().processor().source(item.second.source.get().pixels());
		if(item.second.source.dirty())
		{
			auto node_item = document.inputNodeItem(item.first);
			assert(node_item != nullptr);
			node_item->second.get().forceUpdate();
		}
	});

	std::ranges::for_each(document.palettes(), [&document](auto& item) {
		item.second.processor.get().processor().source(item.second.source.get());
		if(item.second.source.dirty())
		{
			auto node_item = document.inputNodeItem(item.first);
			assert(node_item != nullptr);
			node_item->second.get().forceUpdate();
		}
	});

	document.compositor().process(ret.pixels());
	return ret;
}