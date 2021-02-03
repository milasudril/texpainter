//@	{
//@	 "targets":[{"name":"document.o", "type":"object", "pkgconfig_libs":["wad64"]}]
//@	}

#include "./document.hpp"

#include "pixel_store/image_io.hpp"
#include "imgproc/image_processor_registry.hpp"

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

	nlohmann::json load_document_info(Wad64::ReadonlyArchive const& archive)
	{
		Wad64::InputFile src{archive, "document.json"};
		auto buffer = std::make_unique<char[]>(static_cast<size_t>(src.size()));
		auto range  = std::span{buffer.get(), buffer.get() + src.size()};
		(void)read(src, std::as_writable_bytes(range));
		return nlohmann::json::parse(std::begin(range), std::end(range));
	}

	std::string make_data_path(Texpainter::Model::ItemName const& name)
	{
		return std::string{"data/"} + name.c_str();
	}
}

namespace nlohmann
{
	template<>
	class adl_serializer<Texpainter::Model::Compositor>
	{
	public:
		static void to_json(nlohmann::json& obj, Texpainter::Model::Compositor const& src)
		{
			obj["nodes"] = {};
			auto nodes   = src.nodesWithId();
			std::ranges::for_each(nodes, [&nodes = obj["nodes"]](auto const& item) {
				nodes[toString(item.first)] = item.second;
			});

			obj["connections"] = {};
			std::ranges::for_each(nodes, [&connections = obj["connections"]](auto const& item) {
				std::ranges::for_each(item.second.inputs(), [&conn = connections[toString(item.first)]](auto const& src) {
					if(src.valid())
					{
						conn["node"] = src.processor().nodeId();
						conn["port"] = src.port();
					}
					else
					{
						conn["node"] = Texpainter::FilterGraph::InvalidNodeId;
						conn["port"] = Texpainter::FilterGraph::OutputPortIndex{static_cast<uint32_t>(-1)};
					}
				});
			});
		}
	};
}

std::unique_ptr<Texpainter::Model::Document> Texpainter::Model::load(Enum::Empty<Document>,
                                                                     char const*)
{
	Wad64::FdOwner input_file{"test.tex.wad64", Wad64::IoMode::AllowRead(), load_creation_mode};
	Wad64::ReadonlyArchive archive{std::ref(input_file)};

	auto doc_info = load_document_info(archive);
	auto doc      = std::make_unique<Document>(doc_info.at("canvas_size").get<Size2d>());

	std::map<FilterGraph::NodeId, FilterGraph::NodeId> node_id_map{{FilterGraph::NodeId{0}, FilterGraph::NodeId{0}}};
	std::ranges::for_each(doc_info.at("images").get<std::map<FilterGraph::NodeId, ItemName>>(),
	                      [&archive, document = doc.get(), &node_id_map](auto const& item) {
		                      auto data =
		                          load(Enum::Empty<PixelStore::Image>{},
		                               Wad64::InputFile{archive, make_data_path(item.second)});
		                      auto node_info = document->insert(item.second, std::move(data));
		                      node_id_map[item.first] = node_info->node_id;
	                      });

	std::ranges::for_each(doc_info.at("palettes").get<std::map<FilterGraph::NodeId, ItemName>>(),
	                      [&archive, document = doc.get(), &node_id_map](auto const& item) {
		                      auto data =
		                          load(Enum::Empty<Palette>{},
		                               Wad64::InputFile{archive, make_data_path(item.second)});
		                      auto node_info = document->insert(item.second, std::move(data));
		                      node_id_map[item.first] = node_info->node_id;
	                      });

	{
		auto compositor = doc_info.at("compositor");
		auto nodes      = compositor.at("nodes");
		for(auto it = std::begin(nodes); it != std::end(nodes); ++it)
		{
			auto node_id    = FilterGraph::NodeId{std::stoull(it.key())};
			auto imgproc_id = it.value().at("processor_id").get<FilterGraph::ImageProcessorId>();
			if(node_id == FilterGraph::NodeId{0})  // Skip node id 0
			{
				// But make sure it has the expected ImageProcessorId
				if(imgproc_id != FilterGraph::ImageSink::id())
				{
					throw std::string{"Bad output image processor. Expected "
					                  + toString(FilterGraph::ImageSink::id())};
				}
				continue;
			}

			if(imgproc_id == FilterGraph::InvalidImgProcId) { continue; }

			auto res =
			    doc->compositor().insert(ImageProcessorRegistry::createImageProcessor(imgproc_id));
			std::ranges::for_each(it.value().at("params").get<std::map<std::string, double>>(),
			                      [&node = res.second.get()](auto const& param) {
				                      auto val = std::clamp(param.second, 0.0, 1.0);
				                      node.set(param.first.c_str(), FilterGraph::ParamValue{val});
			                      });

			node_id_map[node_id] = res.first;
		}

		std::ranges::for_each(compositor.at("connections").get<std::map<FilterGraph::NodeId, std::vector<FilterGraph::NodeId>>>(), [&node_id_map, comp = doc->compositor()](auto const& item) mutable {
			auto new_id = node_id_map.find(item.first);
			if(new_id == std::end(node_id_map))
			{ throw std::string{"Connection entry points to a non-exesting node "} + toString(item.first); }

			if(std::size(item.second) >= 4)  // Is it still defined if image processor has fewer inputs
			{ throw std::string{"Too many connections to "} + toString(item.first);}

			std::ranges::for_each(item.second, [comp, sink = new_id->second, &node_id_map, k = 0u](auto id_source) mutable {
				printf("    %s\n", toString(id_source).c_str());

				if(id_source == FilterGraph::InvalidNodeId)
				{ return; }

				auto new_id_source = node_id_map.find(id_source);
				if(new_id_source == std::end(node_id_map))
				{ throw std::string{"Connection entry points to a non-exesting node "} + toString(id_source); }

				// FIXME: Use correct output port index
				comp.connect(sink, FilterGraph::InputPortIndex{k},
							 new_id_source->second, FilterGraph::OutputPortIndex{0});

				++k;
			});
		});
	}

	if(auto i = doc_info.find("workspace"); i != std::end(doc_info))
	{ doc->workspace(i->get<Workspace>()); }

	return doc;
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
		      Wad64::OutputFile{archive, make_data_path(item.first), store_creation_mode});
	});

	std::ranges::for_each(doc.images(), [&archive](auto const& item) {
		store(item.second.source.get(),
		      Wad64::OutputFile{archive, make_data_path(item.first), store_creation_mode});
	});
}