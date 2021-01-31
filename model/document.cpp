//@	{
//@	 "targets":[{"name":"document.o", "type":"object", "pkgconfig_libs":["wad64"]}]
//@	}

#include "./document.hpp"

#include "pixel_store/image_io.hpp"

#include <wad64/archive.hpp>
#include <wad64/readonly_archive.hpp>
#include <wad64/fd_owner.hpp>
#include <wad64/output_file.hpp>
#include <wad64/input_file.hpp>

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
                                                        Document::ForceUpdate force_update,
                                                        uint32_t scale)
{
	std::ranges::for_each(document.images(), [&document, force_update](auto const& item) {
		item.second.processor.get().processor().source(item.second.source.get().pixels());
		forceUpdateIfDirty(item, document, force_update);
	});

	std::ranges::for_each(document.palettes(), [&document, force_update](auto const& item) {
		item.second.processor.get().processor().source(item.second.source.get());
		forceUpdateIfDirty(item, document, force_update);
	});

	PixelStore::Image ret{scale * document.canvasSize().width(),
	                      scale * document.canvasSize().height()};
	if(document.compositor().valid()) [[likely]]
		{
			document.compositor().process(ret.pixels(), static_cast<double>(scale));
		}
	else
	{
		std::ranges::fill(ret.pixels(), PixelStore::Pixel{0.0f, 0.0f, 0.0f, 0.0f});
	}

	std::ranges::for_each(document.images(),
	                      [&document](auto const& item) { item.second.source.clearStatus(); });

	std::ranges::for_each(document.palettes(),
	                      [&document](auto const& item) { item.second.source.clearStatus(); });

	if(scale == 1) [[likely]]
		{
			return ret;
		}

	PixelStore::Image downsampled{document.canvasSize()};
	for(uint32_t row = 0; row < document.canvasSize().height(); ++row)
	{
		for(uint32_t col = 0; col < document.canvasSize().width(); ++col)
		{
			PixelStore::Pixel result{0.0, 0.0, 0.0, 0.0};
			for(uint32_t row_src = 0; row_src < scale; ++row_src)
			{
				for(uint32_t col_src = 0; col_src < scale; ++col_src)
				{
					result += ret(scale * col + col_src, scale * row + row_src);
				}
			}
			downsampled(col, row) = result / static_cast<float>(scale * scale);
		}
	}
	return downsampled;
}

void Texpainter::Model::paint(Document& doc, vec2_t location)
{
	auto brush       = doc.currentBrush();
	auto palette_ref = doc.palette(doc.currentPalette());
	if(palette_ref == nullptr) { return; }

	auto const& palette = palette_ref->source.get();

	doc.modify(
	    [location,
	     brush_radius = static_cast<double>(brush.radius()),
	     brush_func   = BrushFunction{brush.shape()},
	     color        = palette[doc.currentColor()]](PixelStore::Image& img) noexcept {
		    auto r = 0.5 * ScalingFactors::sizeFromGeomMean(img.size(), brush_radius);
		    paint(img.pixels(), location, r, brush_func, color);
		    return true;
	    },
	    doc.currentImage());
}

void Texpainter::Model::paint(Document& doc,
                              vec2_t location,
                              float brush_radius,
                              PixelStore::Pixel color)
{
	doc.modify(
	    [location,
	     brush_radius = static_cast<double>(brush_radius),
	     brush_func   = BrushFunction{doc.currentBrush().shape()},
	     color](PixelStore::Image& img) noexcept {
		    auto r = 0.5 * ScalingFactors::sizeFromGeomMean(img.size(), brush_radius);
		    paint(img.pixels(), location, r, brush_func, color);
		    return true;
	    },
	    doc.currentImage());
}

void Texpainter::Model::floodfill(Document& doc, vec2_t location)
{
	auto palette_ref = doc.palette(doc.currentPalette());
	if(palette_ref == nullptr) [[unlikely]]
		{
			return;
		}

	auto const& palette = palette_ref->source.get();

	doc.modify(
	    [location, color = palette[doc.currentColor()]](PixelStore::Image& img) noexcept {
		    floodfill(img.pixels(), location, color);
		    return true;
	    },
	    doc.currentImage());
}

void Texpainter::Model::floodfill(Document& doc, vec2_t location, PixelStore::Pixel color)
{
	doc.modify(
	    [location, color](PixelStore::Image& img) noexcept {
		    floodfill(img.pixels(), location, color);
		    return true;
	    },
	    doc.currentImage());
}

namespace
{
	constexpr auto load_creation_mode = Wad64::FileCreationMode::DontCare();

	constexpr auto store_creation_mode =
	    Wad64::FileCreationMode::AllowOverwriteWithTruncation().allowCreation();
}

std::unique_ptr<Texpainter::Model::Document> Texpainter::Model::load(Enum::Empty<Document>,
																	 char const*)
{
	Wad64::FdOwner input_file{"test.tex.wad64", Wad64::IoMode::AllowRead(), load_creation_mode};
	Wad64::ReadonlyArchive archive{std::ref(input_file)};
	{
		Wad64::InputFile src{archive, "document.json"};
		auto buffer = std::make_unique<char[]>(static_cast<size_t>(src.size()));
		auto range = std::span{buffer.get(), buffer.get() + src.size()};
		(void)read(src, std::as_writable_bytes(range));
		auto obj = nlohmann::json::parse(std::begin(range), std::end(range));

		auto workspace = obj.at("workspace").get<Workspace>();
	}


	return nullptr;
}

void Texpainter::Model::store(Document const& doc, char const*)
{

	Wad64::FdOwner output_file{
	    "test.tex.wad64", Wad64::IoMode::AllowRead().allowWrite(), store_creation_mode};
	Wad64::Archive archive{std::ref(output_file)};

	{
		nlohmann::json obj{std::pair{"workspace", doc.workspace()},
		                   std::pair{"canvas_size", doc.canvasSize()}};
		obj["compositor"] = doc.compositor();
		obj["images"]     = mapNodeIdsToItemName(doc.images());
		obj["palettes"]   = mapNodeIdsToItemName(doc.palettes());
		auto const str    = obj.dump(1, '\t');

		Wad64::OutputFile output{archive, "document.json", store_creation_mode};
		output.write(std::as_bytes(std::span{str}));
	}

	std::ranges::for_each(doc.palettes(), [&archive](auto const& item) {
		store(item.second.source.get(),
		      Wad64::OutputFile{
		          archive, std::string{"data/"} + item.first.c_str(), store_creation_mode});
	});

	std::ranges::for_each(doc.images(), [&archive](auto const& item) {
		store(item.second.source.get(),
		      Wad64::OutputFile{
		          archive, std::string{"data/"} + item.first.c_str(), store_creation_mode});
	});
}