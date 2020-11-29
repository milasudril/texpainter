//@	{
//@	 "targets":[{"name":"document.o", "type":"object"}]
//@	}

#include "./document.hpp"

#include <algorithm>

namespace
{
	void forceUpdateIfDirty(auto const& item, Texpainter::Model::Document const& doc)
	{
		if(item.second.source.dirty())
		{
			auto node_item = doc.inputNodeItem(item.first);
			assert(node_item != nullptr);
			node_item->second.get().forceUpdate();
		}
	}
}

Texpainter::PixelStore::Image Texpainter::Model::render(Document const& document)
{
	std::ranges::for_each(document.images(), [&document](auto const& item) {
		item.second.processor.get().processor().source(item.second.source.get().pixels());
		forceUpdateIfDirty(item, document);
	});

	std::ranges::for_each(document.palettes(), [&document](auto const& item) {
		item.second.processor.get().processor().source(item.second.source.get());
		forceUpdateIfDirty(item, document);
	});

	PixelStore::Image ret{document.canvasSize()};
	if(document.compositor().valid()) [[likely]]
		{
			document.compositor().process(ret.pixels());
		}
	else
	{ std::ranges::fill(ret.pixels(), PixelStore::Pixel{0.0f, 0.0f, 0.0f, 0.0f}); }
	return ret;
}