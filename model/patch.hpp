//@	{
//@	 "targets":[{"name":"patch.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"patch.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_MODEL_LAYER_HPP
#define TEXPAINTER_MODEL_LAYER_HPP

#include "./brush.hpp"
#include "./item_name.hpp"

#include "pixel_store/image.hpp"
#include "pixel_store/palette.hpp"
#include "pixel_store/image_io.hpp"
#include "utils/angle.hpp"
#include "utils/rect.hpp"

#include <memory>
#include <cassert>

namespace Texpainter::Model
{
	class Patch
	{
	public:
		using Palette = PixelStore::Palette<16>;

		explicit Patch(PixelStore::Image&& img)
		    : m_loc{0.0, 0.0}
		    , m_rot{0}
		    , m_scale{1.0, 1.0}
		    , m_content{std::make_shared<ImageContent>(std::move(img))}
		    , m_update_count{0}
		    , m_current_color{0}
		{
		}

		explicit Patch(Size2d size,
		               PixelStore::Pixel initial_color = PixelStore::Pixel{0.0f, 0.0f, 0.0f, 0.0f})
		    : m_loc{0.0, 0.0}
		    , m_rot{0}
		    , m_scale{1.0, 1.0}
		    , m_content{std::make_shared<ImageContent>(size)}
		    , m_update_count{0}
		    , m_current_color{0}
		{
			fill(initial_color);
		}

		Patch(Patch const& other) = delete;

		~Patch() = default;

		Patch(Patch&&) = default;

		Patch& operator=(Patch&&) = default;

		Patch& operator=(Patch const& other) = delete;

		Patch linkedPatch() const
		{
			return Patch{m_loc, m_rot, m_scale, m_content, m_current_color};
		}

		Patch copiedPatch() const
		{
			return Patch{
			    m_loc, m_rot, m_scale, std::make_shared<ImageContent>(*m_content), m_current_color};
		}

		Patch& convertToCopy()
		{
			m_content = std::make_shared<ImageContent>(*m_content);
			return *this;
		}

		Patch& paint(vec2_t origin, float radius, BrushFunction brush, PixelStore::Pixel color);

		Patch& fill(PixelStore::Pixel color)
		{
			std::ranges::fill(m_content->pixels(), color);
			return *this;
		}

		PixelStore::Image const& content() const { return m_content->base(); }

		vec2_t location() const { return m_loc; }

		Patch& location(vec2_t loc)
		{
			m_loc = loc;
			return *this;
		}

		Angle rotation() const { return m_rot; }

		Patch& rotation(Angle rot)
		{
			m_rot = rot;
			return *this;
		}

		vec2_t scaleFactor() const { return m_scale; }

		Patch& scaleFactor(vec2_t val)
		{
			m_scale = val;
			return *this;
		}

		auto size() const { return m_content->size(); }

		bool hasNewContent() const { return m_update_count < m_content->updateCount(); }


		auto const& palette() const { return m_palette; }

		Patch& palette(Palette const& pal)
		{
			m_palette      = pal;
			m_update_count = 0;
			return *this;
		}

		Patch& colorModify(PixelStore::ColorIndex index, PixelStore::Pixel value)
		{
			assert(static_cast<size_t>(index.value()) < m_palette.size());
			m_palette[index] = value;
			m_update_count   = 0;
			return *this;
		}

		auto currentColor() const { return m_current_color; }

		Patch& currentColor(PixelStore::ColorIndex index)
		{
			m_current_color = index;
			return *this;
		}

	private:
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
		mutable size_t m_update_count;
		PixelStore::ColorIndex m_current_color;

		explicit Patch(vec2_t loc,
		               Angle rot,
		               vec2_t scale,
		               std::shared_ptr<ImageContent> const& content,
		               PixelStore::ColorIndex current_color = PixelStore::ColorIndex{0})
		    : m_loc{loc}
		    , m_rot{rot}
		    , m_scale{scale}
		    , m_content{content}
		    , m_update_count{0}
		    , m_current_color{current_color}
		{
		}
	};

	inline vec2_t axisAlignedBoundingBox(Patch const& patch, double scale = 1.0)
	{
		auto const size = patch.content().size();
		auto const scaled_size =
		    scale * vec2_t{static_cast<double>(size.width()), static_cast<double>(size.height())}
		    * patch.scaleFactor();

		return axisAlignedBoundingBox(scaled_size, patch.rotation());
	}

	void render(Patch const& patch, Span2d<PixelStore::Pixel> ret, double scale = 1.0);

	void outline(Patch const& patch, Span2d<PixelStore::Pixel> ret);

	inline auto currentColor(Patch const& patch) { return patch.palette()[patch.currentColor()]; }

	inline std::pair<ItemName, Patch> createPatchFromFile(char const* filename)
	{
		auto patch = Patch{PixelStore::load(filename)};
		return std::make_pair(createItemNameFromFilename(filename),
		                      Patch{PixelStore::load(filename)});
	}
}

#endif
