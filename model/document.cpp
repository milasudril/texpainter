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

	std::string make_data_path(Texpainter::Model::ItemName const& name)
	{
		return std::string{"data/"} + name.c_str();
	}

	nlohmann::json serialize(Texpainter::Model::Compositor const& compositor)
	{
		return nodeData(compositor);
	}

	using NodeIdMap = std::map<Texpainter::FilterGraph::NodeId, Texpainter::FilterGraph::NodeId>;

	template<class T>
	class LoadItem
	{
	public:
		explicit LoadItem(std::reference_wrapper<Wad64::ReadonlyArchive> archive,
			std::reference_wrapper<Texpainter::Model::Document> doc,
			std::reference_wrapper<NodeIdMap> id_map):
			m_archive{archive},
			m_doc{doc},
			m_id_map{id_map}
		{}

		void operator()(auto const& item) const
		{
			auto data = load(Enum::Empty<T>{},
		                               Wad64::InputFile{m_archive.get(), make_data_path(item.second)});
			auto node_info = m_doc.get().insert(item.second, std::move(data));
		    m_id_map.get()[item.first] = node_info->node_id;
		}

	private:
		std::reference_wrapper<Wad64::ReadonlyArchive> m_archive;
		std::reference_wrapper<Texpainter::Model::Document> m_doc;
		std::reference_wrapper<NodeIdMap> m_id_map;
	};

	nlohmann::json load_document_info(Wad64::ReadonlyArchive const& archive)
	{
		Wad64::InputFile src{archive, "document.json"};
		auto buffer = std::make_unique<char[]>(static_cast<size_t>(src.size()));
		auto range  = std::span{buffer.get(), buffer.get() + src.size()};
		(void)read(src, std::as_writable_bytes(range));
		return nlohmann::json::parse(std::begin(range), std::end(range));
	}



	template<class NodeMap, class Compositor>
	void load_nodes(NodeMap const& nodes, Compositor compositor, NodeIdMap& id_map)
	{
		std::ranges::for_each(nodes, [compositor, &id_map](auto const& item) mutable {
			if(item.first == Texpainter::FilterGraph::NodeId{0})
			{
				if(item.second.imgproc != Texpainter::FilterGraph::ImageSink::id())
				{
					throw std::string{"Bad output image processor. Expected "
					                  + toString(Texpainter::FilterGraph::ImageSink::id())};
				}
				return;
			}

			if(item.second.imgproc == Texpainter::FilterGraph::InvalidImgProcId)
			{ return; }

			auto res = compositor.insert(Texpainter::ImageProcessorRegistry::createImageProcessor(item.second.imgproc));
			std::ranges::for_each(item.second.params, [&node = res.second.get()](auto const& param) {
				auto val = std::clamp(param.second, 0.0, 1.0);
				node.set(param.first.c_str(), Texpainter::FilterGraph::ParamValue{val});
			});

			id_map[item.first] = res.first;
		});
	}

	template<class NodeMap, class Compositor>
	void connect_nodes(NodeMap const& nodes, Compositor compositor, NodeIdMap const& id_map)
	{
		std::ranges::for_each(nodes, [compositor, &id_map](auto const& item) mutable {
			auto const id_mapping = id_map.find(item.first);
			if(id_mapping == std::end(id_map))
			{ throw std::string{"Connection entry points to a non-exesting node "} + toString(item.first); }

			std::ranges::for_each(item.second.inputs, [compositor,
								  sink = id_mapping->second, &id_map, k = 0u](auto const& src) mutable {
				printf("    %s\n", toString(src.node).c_str());

				if(src.node == Texpainter::FilterGraph::InvalidNodeId)
				{ return; }

				auto const id_mapping = id_map.find(src.node);
				if(id_mapping == std::end(id_map))
				{ throw std::string{"Connection entry points to a non-exesting node "} + toString(src.node); }

				auto const source = id_mapping->second;
				compositor.connect(sink, Texpainter::FilterGraph::InputPortIndex{k}, source, src.output_port);
				++k;
			});
		});
	}
}

std::unique_ptr<Texpainter::Model::Document> Texpainter::Model::load(Enum::Empty<Document>,
                                                                     char const*)
{
	Wad64::FdOwner input_file{"test.tex.wad64", Wad64::IoMode::AllowRead(), load_creation_mode};
	Wad64::ReadonlyArchive archive{std::ref(input_file)};

	auto doc_info = load_document_info(archive);
	auto doc      = std::make_unique<Document>(doc_info.at("canvas_size").get<Size2d>());

	NodeIdMap id_map{{FilterGraph::NodeId{0}, FilterGraph::NodeId{0}}};

	using NodeIdItemNameMap = std::map<FilterGraph::NodeId, ItemName>;

	std::ranges::for_each(doc_info.at("images").get<NodeIdItemNameMap>(),
						  LoadItem<PixelStore::Image>{archive, *doc, id_map});

	std::ranges::for_each(doc_info.at("palettes").get<NodeIdItemNameMap>(),
						  LoadItem<Palette>{archive, *doc, id_map});

	auto compositor_data = doc_info.at("compositor").get<std::map<FilterGraph::NodeId, FilterGraph::NodeData>>();

	load_nodes(compositor_data, doc->compositor(), id_map);

	connect_nodes(compositor_data, doc->compositor(), id_map);

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
		obj["compositor"] = serialize(doc.compositor());
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