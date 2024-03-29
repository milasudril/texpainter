//@	{
//@	 "targets":[{"name":"document.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"document.o", "rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_MODEL_DOCUMENT_HPP
#define TEXPAINTER_MODEL_DOCUMENT_HPP

#include "./compositor_proxy.hpp"
#include "./compositor_input.hpp"
#include "./item_name.hpp"
#include "./image_source.hpp"
#include "./palette_source.hpp"
#include "./topography_info_source.hpp"
#include "./compositor_proxy.hpp"
#include "./brush.hpp"
#include "./paint.hpp"
#include "./workspace.hpp"

#include "pixel_store/rgba_image.hpp"
#include "pixel_store/topographic_map.hpp"
#include "utils/with_status.hpp"

#include <algorithm>
#include <iterator>

namespace Texpainter::Model
{
	namespace detail
	{
		template<>
		struct ImageProcessor<WithStatus<PixelStore::RgbaImage>>
		{
			using type = ImageSource;
		};

		template<>
		struct ImageProcessor<WithStatus<Palette>>
		{
			using type = PaletteSource;
		};

		template<>
		struct ImageProcessor<WithStatus<PixelStore::TopographicMap>>
		{
			using type = TopographyInfoSource;
		};
	}

	class Document: private Size2d,
	                private Compositor,
	                private CompositorInputManager<PixelStore::RgbaImage>,
	                private CompositorInputManager<Palette>,
	                private CompositorInputManager<PixelStore::TopographicMap>
	{
		using CompositorInputManager<PixelStore::RgbaImage>::insert;
		using CompositorInputManager<PixelStore::RgbaImage>::erase;
		using CompositorInputManager<Palette>::insert;
		using CompositorInputManager<Palette>::erase;
		using CompositorInputManager<PixelStore::TopographicMap>::insert;
		using CompositorInputManager<PixelStore::TopographicMap>::erase;


	public:
		class ForceUpdate
		{
		public:
			constexpr explicit ForceUpdate(bool status): m_status{status} {}
			constexpr operator bool() const { return m_status; }

		private:
			bool m_status;
		};

		using CompositorInputManager<PixelStore::RgbaImage>::get;
		using CompositorInputManager<PixelStore::RgbaImage>::getBefore;
		using CompositorInputManager<PixelStore::RgbaImage>::getAfter;
		using CompositorInputManager<PixelStore::RgbaImage>::modify;
		using CompositorInputManager<Palette>::get;
		using CompositorInputManager<Palette>::getBefore;
		using CompositorInputManager<Palette>::getAfter;
		using CompositorInputManager<Palette>::modify;
		using CompositorInputManager<PixelStore::TopographicMap>::get;
		using CompositorInputManager<PixelStore::TopographicMap>::getBefore;
		using CompositorInputManager<PixelStore::TopographicMap>::getAfter;
		using CompositorInputManager<PixelStore::TopographicMap>::modify;

		explicit Document(Size2d canvas_size): Size2d{canvas_size} {}

		Size2d canvasSize() const { return static_cast<Size2d>(*this); }

		Document& canvasSize(Size2d size)
		{
			static_cast<Size2d&>(*this) = size;
			return *this;
		}

		Compositor const& compositor() const { return static_cast<Compositor const&>(*this); }

		CompositorProxy<Document> compositor()
		{
			return CompositorProxy{*this, static_cast<Compositor&>(*this)};
		}


		template<class T>
		auto insert(Model::ItemName const&, T&&, Size2d)
		{
			return nullptr;
		}

		template<class T>
		auto insert(Model::ItemName const&, PixelStore::Image<T>&&)
		{
			return nullptr;
		}


		auto const& images() const { return get(std::type_identity<PixelStore::RgbaImage>{}); }

		auto image(ItemName const& name) const
		{
			return get(std::type_identity<PixelStore::RgbaImage>{}, name);
		}

		auto insert(ItemName const& name, PixelStore::RgbaImage&& img)
		{
			return insert(name, std::move(img), *this, m_input_nodes);
		}

		bool eraseImage(ItemName const& name)
		{
			return erase(std::type_identity<PixelStore::RgbaImage>{}, name, *this, m_input_nodes);
		}


		auto const& topographicMaps() const
		{
			return get(std::type_identity<PixelStore::TopographicMap>{});
		}

		auto topographicMap(ItemName const& name) const
		{
			return get(std::type_identity<PixelStore::TopographicMap>{}, name);
		}

		auto insert(ItemName const& name, PixelStore::TopographicMap&& img)
		{
			return insert(name, std::move(img), *this, m_input_nodes);
		}

		bool eraseTopographicMap(ItemName const& name)
		{
			return erase(
			    std::type_identity<PixelStore::TopographicMap>{}, name, *this, m_input_nodes);
		}


		auto const& palettes() const { return get(std::type_identity<Palette>{}); }

		auto palette(ItemName const& name) const
		{
			return get(std::type_identity<Palette>{}, name);
		}

		auto insert(ItemName const& name, Palette&& pal)
		{
			if(palettes().size() == 0) { currentColor(PixelStore::ColorIndex{0}); }
			return insert(name, std::move(pal), *this, m_input_nodes);
		}

		bool erasePalette(ItemName const& name)
		{
			return erase(std::type_identity<Palette>{}, name, *this, m_input_nodes);
		}

		Compositor::NodeItem const* inputNodeItem(ItemName const& name) const
		{
			auto i = m_input_nodes.find(name);
			return i != std::end(m_input_nodes) ? &i->second : nullptr;
		}

		ItemName const* inputNodeName(Compositor::NodeId id) const
		{
			auto i = std::ranges::find_if(
			    m_input_nodes, [id](auto const& item) { return item.second.first == id; });
			return i != std::end(m_input_nodes) ? &i->first : nullptr;
		}

		auto erase(Compositor::NodeId id)
		{
			auto name = inputNodeName(id);
			if(name == nullptr) [[likely]]
				{
					Compositor::erase(id);
					return true;
				}

			if(!eraseImage(*name)) { return erasePalette(*name); }

			return true;
		}

		auto copy(Compositor::NodeId id, ItemName const& name_new)
		{
			auto name = inputNodeName(id);
			assert(name != nullptr);

			if(auto img = image(*name); img != nullptr)
			{
				insert(name_new, Texpainter::PixelStore::RgbaImage{img->source.get()});
				return inputNodeItem(name_new);
			}

			if(auto pal = palette(*name); pal != nullptr)
			{
				insert(name_new, Texpainter::PixelStore::Palette{pal->source.get()});
				return inputNodeItem(name_new);
			}
			__builtin_unreachable();
		}

		auto const& nodeLocations() const { return m_workspace.m_node_locations; }

		Document& nodeLocations(std::map<FilterGraph::NodeId, vec2_t>&& locs)
		{
			m_workspace.m_node_locations = std::move(locs);
			return *this;
		}

		BrushInfo currentBrush() const { return m_workspace.m_current_brush; }

		Document& currentBrush(BrushInfo brush)
		{
			m_workspace.m_current_brush = brush;
			return *this;
		}

		ItemName const& currentImage() const { return m_workspace.m_current_image; }

		Document& currentImage(ItemName&& name)
		{
			m_workspace.m_current_image = std::move(name);
			return *this;
		}

		PixelStore::ColorIndex currentColor() const { return m_workspace.m_current_color; }

		Document& currentColor(PixelStore::ColorIndex i)
		{
			m_workspace.m_current_color = i;
			return *this;
		}

		ItemName const& currentPalette() const { return m_workspace.m_current_palette; }

		Document& currentPalette(ItemName&& name)
		{
			m_workspace.m_current_palette = std::move(name);
			return *this;
		}

		PixelStore::RgbaPalette<8> const& colorHistory() const
		{
			return m_workspace.m_color_history;
		}

		Document& saveColor(PixelStore::RgbaValue color)
		{
			std::rotate(std::rbegin(m_workspace.m_color_history),
			            std::rbegin(m_workspace.m_color_history) + 1,
			            std::rend(m_workspace.m_color_history));
			m_workspace.m_color_history[PixelStore::ColorIndex{0}] = color;
			return *this;
		}

		std::filesystem::path const& workingDirectory() const
		{
			return m_workspace.m_working_directory;
		}

		Workspace const& workspace() const { return m_workspace; }

		Document& workspace(Workspace&& obj)
		{
			m_workspace = std::move(obj);
			return *this;
		}

		template<class T>
		ItemName const& current() const;

		template<class T>
		Document& current(ItemName&&);

		Document& windows(Windows&& windows)
		{
			m_workspace.m_windows = std::move(windows);
			return *this;
		}

		Document& filename(std::filesystem::path&& filename)
		{
			m_filename                      = std::move(filename);
			m_workspace.m_working_directory = m_filename.parent_path();
			return *this;
		}

		std::filesystem::path const& filename() const { return m_filename; }

	private:
		std::map<ItemName, Compositor::NodeItem> m_input_nodes;

		Workspace m_workspace;
		std::filesystem::path m_filename;
	};

	CompositorOutput render(Document const& document,
	                        Document::ForceUpdate foce_update = Document::ForceUpdate{false},
	                        uint32_t scale                    = 1);

	void paint(Document& doc, vec2_t location);

	void paint(Document& doc, vec2_t location, float brush_radius, PixelStore::RgbaValue color);

	void floodfill(Document& doc, vec2_t location);

	void floodfill(Document& doc, vec2_t location, PixelStore::RgbaValue color);

	std::unique_ptr<Document> load(Enum::Empty<Document>, char const* filename);

	void store(Document const& doc, char const* filename);

	bool fileValid(Enum::Empty<Document>, char const* filename);

	template<>
	inline ItemName const& Document::current<PixelStore::RgbaImage>() const
	{
		return currentImage();
	}

	template<>
	inline Document& Document::current<PixelStore::RgbaImage>(ItemName&& name)
	{
		currentImage(std::move(name));
		return *this;
	}

	template<>
	inline ItemName const& Document::current<Palette>() const
	{
		return currentPalette();
	}

	template<>
	inline Document& Document::current<Palette>(ItemName&& name)
	{
		currentPalette(std::move(name));
		return *this;
	}
}

#endif