//@	{
//@	 "targets":[{"name":"layer.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"layer.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_MODEL_LAYER_HPP
#define TEXPAINTER_MODEL_LAYER_HPP

#include "./brush.hpp"
#include "./compositing_options.hpp"
#include "./item_name.hpp"

#include "pixel_store/image.hpp"
#include "pixel_store/palette.hpp"
#include "pixel_store/image_io.hpp"
#include "utils/angle.hpp"
#include "utils/rect.hpp"
#include "filtergraph/graph.hpp"

#include <memory>

namespace Texpainter::Model
{
	class Layer
	{
	public:
		using Palette = PixelStore::Palette<16>;

		explicit Layer(PixelStore::Image&& img)
		    : m_visibility_flags{Visible}
		    , m_loc{0.0, 0.0}
		    , m_rot{0}
		    , m_scale{1.0, 1.0}
		    , m_content{std::make_shared<ImageContent>(std::move(img))}
		    , m_content_filtered{m_content->size()}
		    , m_update_count{0}
		    , m_current_color{0}
		{
		}

		explicit Layer(Size2d size,
		               PixelStore::Pixel initial_color = PixelStore::Pixel{0.0f, 0.0f, 0.0f, 0.0f})
		    : m_visibility_flags{Visible}
		    , m_loc{0.0, 0.0}
		    , m_rot{0}
		    , m_scale{1.0, 1.0}
		    , m_content{std::make_shared<ImageContent>(size)}
		    , m_content_filtered{m_content->size()}
		    , m_update_count{0}
		    , m_current_color{0}
		{
			fill(initial_color);
		}

		Layer(Layer const& other) = delete;

		~Layer() = default;

		Layer(Layer&&) = default;

		Layer& operator=(Layer&&) = default;

		Layer& operator=(Layer const& other) = delete;

		Layer linkedLayer() const
		{
			return Layer{m_visibility_flags,
			             m_loc,
			             m_rot,
			             m_scale,
			             m_content,
			             m_compose_opts,
			             m_current_color};
		}

		Layer copiedLayer() const
		{
			return Layer{m_visibility_flags,
			             m_loc,
			             m_rot,
			             m_scale,
			             std::make_shared<ImageContent>(*m_content),
			             m_compose_opts,
			             m_current_color};
		}

		Layer& convertToCopy()
		{
			m_content = std::make_shared<ImageContent>(*m_content);
			return *this;
		}

		Layer& paint(vec2_t origin, float radius, BrushFunction brush, PixelStore::Pixel color);

		Layer& fill(PixelStore::Pixel color)
		{
			std::ranges::fill(m_content->pixels(), color);
			return *this;
		}

		PixelStore::Image const& content() const { return m_content->base(); }

		vec2_t location() const { return m_loc; }

		Layer& location(vec2_t loc)
		{
			m_loc = loc;
			return *this;
		}

		Angle rotation() const { return m_rot; }

		Layer& rotation(Angle rot)
		{
			m_rot = rot;
			return *this;
		}

		vec2_t scaleFactor() const { return m_scale; }

		Layer& scaleFactor(vec2_t val)
		{
			m_scale = val;
			return *this;
		}

		bool visible() const { return m_visibility_flags & Visible; }

		Layer& visible(bool val)
		{
			m_visibility_flags =
			    val ? m_visibility_flags | Visible : m_visibility_flags & (~Visible);
			return *this;
		}

		bool isolated() const { return m_visibility_flags & Isolated; }

		Layer& isolated(bool val)
		{
			m_visibility_flags =
			    val ? m_visibility_flags | Isolated : m_visibility_flags & (~Isolated);
			return *this;
		}

		auto size() const { return m_content->size(); }

		Compositor const& filterGraph() const { return m_compose_opts.filterGraph(); }

		Layer& filterGraph(Compositor&& graph)
		{
			m_compose_opts.filterGraph(std::move(graph));
			return *this;
		}

		Layer& nodeLocations(std::map<FilterGraph::NodeId, vec2_t>&& loc)
		{
			m_node_locations = std::move(loc);
			return *this;
		}

		std::map<FilterGraph::NodeId, vec2_t> const& nodeLocations() const
		{
			return m_node_locations;
		}

		Layer& compositingOptions(CompositingOptions&& opts)
		{
			m_compose_opts = std::move(opts);
			m_update_count = 0;
			return *this;
		}

		CompositingOptions const& compositingOptions() const { return m_compose_opts; }

		PixelStore::Image const& filteredContent(Compositor const* filter) const
		{
			if(filter == nullptr) { return content(); }

			if(m_update_count >= m_content->updateCount()
			   && filter == &m_compose_opts.filterGraph())
			{ return m_content_filtered; }

			m_content_filtered = filter->process(FilterGraph::Input{content().pixels(), m_palette});
			m_update_count     = m_content->updateCount();
			return m_content_filtered;
		}


		auto const& palette() const { return m_palette; }

		Layer& palette(Palette const& pal)
		{
			m_palette      = pal;
			m_update_count = 0;
			return *this;
		}

		Layer& colorModify(PixelStore::ColorIndex index, PixelStore::Pixel value)
		{
			assert(static_cast<size_t>(index.value()) < m_palette.size());
			m_palette[index] = value;
			m_update_count   = 0;
			return *this;
		}

		auto currentColor() const { return m_current_color; }

		Layer& currentColor(PixelStore::ColorIndex index)
		{
			m_current_color = index;
			return *this;
		}

	private:
		static constexpr size_t Visible  = 0x1;
		static constexpr size_t Isolated = 0x2;

		size_t m_visibility_flags;
		vec2_t m_loc;
		Angle m_rot;
		vec2_t m_scale;

		class ImageContent: PixelStore::Image
		{
		public:
			using PixelStore::Image::height;
			using PixelStore::Image::pixels;
			using PixelStore::Image::size;
			using PixelStore::Image::width;

			explicit ImageContent(Size2d size): PixelStore::Image{size}, m_update_count{1} {}

			explicit ImageContent(PixelStore::Image&& img)
			    : PixelStore::Image{std::move(img)}
			    , m_update_count{1}
			{
			}

			void incUpdateCount() { ++m_update_count; }

			size_t updateCount() const { return m_update_count; }

			PixelStore::Image const& base() const { return *this; }

		private:
			size_t m_update_count;
		};

		std::shared_ptr<ImageContent> m_content;
		Palette m_palette;
		CompositingOptions m_compose_opts;
		mutable PixelStore::Image m_content_filtered;
		mutable size_t m_update_count;
		std::map<FilterGraph::NodeId, vec2_t> m_node_locations;
		PixelStore::ColorIndex m_current_color;

		explicit Layer(size_t vis,
		               vec2_t loc,
		               Angle rot,
		               vec2_t scale,
		               std::shared_ptr<ImageContent> const& content,
		               CompositingOptions const& compose_opts,
		               PixelStore::ColorIndex current_color = PixelStore::ColorIndex{0})
		    : m_visibility_flags{vis}
		    , m_loc{loc}
		    , m_rot{rot}
		    , m_scale{scale}
		    , m_content{content}
		    , m_compose_opts{compose_opts}
		    , m_content_filtered{m_content->size()}
		    , m_update_count{0}
		    , m_current_color{current_color}
		{
		}
	};

	inline vec2_t axisAlignedBoundingBox(Layer const& layer, double scale = 1.0)
	{
		auto const size = layer.content().size();
		auto const scaled_size =
		    scale * vec2_t{static_cast<double>(size.width()), static_cast<double>(size.height())}
		    * layer.scaleFactor();

		return axisAlignedBoundingBox(scaled_size, layer.rotation());
	}

	void render(Layer const& layer,
	            Span2d<PixelStore::Pixel> ret,
	            BlendFunction blend,
	            float opacity,
	            Compositor const* filter,
	            double scale = 1.0);

	inline void render(Layer const& layer,
	                   Span2d<PixelStore::Pixel> ret,
	                   CompositingOptions const& compose_opts,
	                   double scale = 1.0)
	{
		auto const& filter_graph = compose_opts.filterGraph();
		render(layer,
		       ret,
		       compose_opts.blendFunction(),
		       compose_opts.opacity(),
		       filter_graph.valid() ? &filter_graph : nullptr,
		       scale);
	}

	inline void render(Layer const& layer, Span2d<PixelStore::Pixel> ret, double scale = 1.0)
	{
		render(layer, ret, layer.compositingOptions(), scale);
	}

	void outline(Layer const& layer, Span2d<PixelStore::Pixel> ret);

	inline auto currentColor(Layer const& layer) { return layer.palette()[layer.currentColor()]; }

	inline std::pair<ItemName, Layer> createLayerFromFile(char const* filename)
	{
		auto layer = Layer{PixelStore::load(filename)};
		return std::make_pair(createItemNameFromFilename(filename),
		                      Layer{PixelStore::load(filename)});
	}
}

#endif
