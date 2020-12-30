//@	{
//@	 "targets":[{"name":"document.o", "type":"object"}]
//@	}

#include "./document.hpp"

#include <algorithm>

namespace
{
	void forceUpdateIfDirty(auto const& item,
	                        Texpainter::Model::Document const& doc,
	                        Texpainter::Model::Document::ForceUpdate force_update)
	{
		if(item.second.source.dirty() || force_update)
		{
			auto node_item = doc.inputNodeItem(item.first);
			assert(node_item != nullptr);
			node_item->second.get().forceUpdate();
		}
	}
}

Texpainter::PixelStore::Image Texpainter::Model::render(Document const& document,
                                                        Document::ForceUpdate force_update)
{
	std::ranges::for_each(document.images(), [&document, force_update](auto const& item) {
		item.second.processor.get().processor().source(item.second.source.get().pixels());
		forceUpdateIfDirty(item, document, force_update);
	});

	std::ranges::for_each(document.palettes(), [&document, force_update](auto const& item) {
		item.second.processor.get().processor().source(item.second.source.get());
		forceUpdateIfDirty(item, document, force_update);
	});

	PixelStore::Image ret{document.canvasSize()};
	if(document.compositor().valid()) [[likely]]
		{
			document.compositor().process(ret.pixels());
		}
	else
	{
		std::ranges::fill(ret.pixels(), PixelStore::Pixel{0.0f, 0.0f, 0.0f, 0.0f});
	}

	if(!force_update) [[likely]]
		{
			// In case force_update is true, it could be possible that we are rendering with a larger
			// canvas because we are doing a detailed render. In that case do not clear the status flag.
			// Maybe we should compare the output canvas size with the document canvas size.

			std::ranges::for_each(document.images(), [&document](auto const& item) {
				item.second.source.clearStatus();
			});

			std::ranges::for_each(document.palettes(), [&document](auto const& item) {
				item.second.source.clearStatus();
			});
		}
	return ret;
}

void Texpainter::Model::paint(Document& doc, vec2_t location)
{
	auto brush       = doc.currentBrush();
	auto palette_ref = doc.palette(doc.currentPalette());
	if(palette_ref == nullptr) [[unlikely]]
		{
			return;
		}

	auto const& palette = palette_ref->source.get();

	doc.modify(
	    [location,
	     brush_radius = static_cast<double>(brush.radius()),
	     brush_func   = BrushFunction{brush.shape()},
	     color        = palette[doc.currentColor()]](PixelStore::Image& img) noexcept {
		    auto r = 0.5 * std::exp2(brush_radius * 0.5 * std::log2(img.area()));
		    paint(img.pixels(), location, r, brush_func, color);
		    return true;
	    },
	    doc.currentImage());
}

void Texpainter::Model::store(Document const& doc, char const* filename)
{
	nlohmann::json obj{std::pair{"workspace", doc.workspace()}};
	auto const str = obj.dump(1, '\t');

	auto const f = fopen(filename, "wb");
	if(f == nullptr) { throw std::string{"Failed to open "} + filename + ": " + strerror(errno); }
	fputs(str.c_str(), f);
	fclose(f);
}