//@	{
//@	 "targets":[{"name":"patch.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"patch.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_MODEL_PATCH_HPP
#define TEXPAINTER_MODEL_PATCH_HPP

#include "./brush.hpp"
#include "./item_name.hpp"

#include "pixel_store/image.hpp"
#include "pixel_store/palette.hpp"
#include "pixel_store/image_io.hpp"
#include "utils/angle.hpp"
#include "utils/rect.hpp"
#include "filtergraph/port_info.hpp"
#include "filtergraph/port_type.hpp"
#include "filtergraph/img_proc_param.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"

#include <memory>
#include <cassert>

namespace Texpainter::Model
{
	class Patch;

	void render(Patch const& patch, Span2d<PixelStore::Pixel> ret, double scale = 1.0);

	class Patch
	{
	public:
		using Palette = PixelStore::Palette<16>;

		struct InterfaceDescriptor
		{
			static constexpr std::array<FilterGraph::PortInfo, 0> InputPorts{};
			static constexpr std::array<FilterGraph::PortInfo, 2> OutputPorts{
			    {{FilterGraph::PortType::RgbaPixels, "Pixels"},
			     {FilterGraph::PortType::Palette, "Palette"}}};
			static constexpr std::array<FilterGraph::ParamName, 0> ParamNames{};
		};

		explicit Patch(PixelStore::Image&& img)
		    : m_loc{0.0, 0.0}
		    , m_rot{0}
		    , m_scale{1.0, 1.0}
		    , m_image{std::move(img)}
		    , m_current_color{0}
		{
		}

		explicit Patch(Size2d size,
		               PixelStore::Pixel initial_color = PixelStore::Pixel{0.0f, 0.0f, 0.0f, 0.0f})
		    : m_loc{0.0, 0.0}
		    , m_rot{0}
		    , m_scale{1.0, 1.0}
		    , m_image{size}
		    , m_current_color{0}
		{
			fill(initial_color);
		}

		Patch& paint(vec2_t origin, float radius, BrushFunction brush, PixelStore::Pixel color);

		Patch& fill(PixelStore::Pixel color)
		{
			std::ranges::fill(m_image.pixels(), color);
			return *this;
		}

		PixelStore::Image const& image() const { return m_image; }

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

		auto size() const { return m_image.size(); }

		auto const& palette() const { return m_palette; }

		Patch& palette(Palette const& pal)
		{
			m_palette = pal;
			return *this;
		}

		Patch& colorModify(PixelStore::ColorIndex index, PixelStore::Pixel value)
		{
			assert(static_cast<size_t>(index.value()) < m_palette.size());
			m_palette[index] = value;
			return *this;
		}

		auto currentColor() const { return m_current_color; }

		Patch& currentColor(PixelStore::ColorIndex index)
		{
			m_current_color = index;
			return *this;
		}

		void operator()(FilterGraph::ImgProcArg<InterfaceDescriptor> const& arg) const
		{
			render(*this, Texpainter::Span2d{arg.output<0>(), arg.size()});
			arg.output<1>().get() = m_palette;
		}

		std::span<FilterGraph::ParamValue> paramValues() const
		{
			return std::span<FilterGraph::ParamValue>{};
		}

		FilterGraph::ParamValue get(FilterGraph::ParamName) const
		{
			return FilterGraph::ParamValue{0};
		}

		void set(FilterGraph::ParamName, FilterGraph::ParamValue) {}

		char const* name() const { return ""; }

		static constexpr FilterGraph::ImageProcessorId id()
		{
			return FilterGraph::ImageProcessorId{"025f50bc6ebc706daae991fc9679185c"};
		}


	private:
		vec2_t m_loc;
		Angle m_rot;
		vec2_t m_scale;
		PixelStore::Image m_image;
		Palette m_palette;
		PixelStore::ColorIndex m_current_color;
	};

	inline vec2_t axisAlignedBoundingBox(Patch const& patch, double scale = 1.0)
	{
		auto const size = patch.image().size();
		auto const scaled_size =
		    scale * vec2_t{static_cast<double>(size.width()), static_cast<double>(size.height())}
		    * patch.scaleFactor();

		return axisAlignedBoundingBox(scaled_size, patch.rotation());
	}

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
