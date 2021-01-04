//@	{
//@	 "targets":[{"name":"image_source.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_MODEL_IMAGESOURCE_HPP
#define TEXPAINTER_MODEL_IMAGESOURCE_HPP

#include "filtergraph/port_info.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"

#include "utils/span_2d.hpp"
#include "utils/rect.hpp"

#include <cassert>
#include <string>

namespace Texpainter::Model
{
	class ImageSource
	{
	public:
		struct InterfaceDescriptor
		{
			static constexpr std::array<FilterGraph::PortInfo, 0> InputPorts{};
			static constexpr std::array<FilterGraph::PortInfo, 1> OutputPorts{
			    {{FilterGraph::PortType::RgbaPixels, "Pixels"}}};
			static constexpr std::array<FilterGraph::ParamName, 0> ParamNames{};
		};

		explicit ImageSource(std::string&& name): m_name{std::move(name)} {}

		void operator()(FilterGraph::ImgProcArg<InterfaceDescriptor> const& args) const;

		FilterGraph::ParamValue get(FilterGraph::ParamName) const
		{
			return FilterGraph::ParamValue{0.0};
		}

		void set(FilterGraph::ParamName, FilterGraph::ParamValue) {}

		std::span<FilterGraph::ParamValue const> paramValues() const
		{
			return std::span<FilterGraph::ParamValue const>{};
		}

		char const* name() const { return m_name.c_str(); }

		static constexpr auto id() { return FilterGraph::InvalidImgProcId; }

		ImageSource& source(Span2d<FilterGraph::RgbaValue const> src)
		{
			r_pixels = src;
			return *this;
		}

	private:
		Span2d<FilterGraph::RgbaValue const> r_pixels;
		std::string m_name;
	};
}

inline void Texpainter::Model::ImageSource::operator()(
    FilterGraph::ImgProcArg<InterfaceDescriptor> const& args) const
{
	auto ret = Texpainter::Span2d{args.template output<0>(), args.canvasSize()};

	auto const src_ratio  = aspectRatio(r_pixels);
	auto const sink_ratio = aspectRatio(ret);
	auto const r          = src_ratio > sink_ratio
	                   ? static_cast<double>(ret.width()) / static_cast<double>(r_pixels.width())
	                   : static_cast<double>(ret.height()) / static_cast<double>(r_pixels.height());
	auto const scale_factor = 1 / r;

	auto const src = r_pixels;
	auto const origin_src =
	    0.5 * vec2_t{static_cast<double>(src.width()), static_cast<double>(src.height())};
	auto const loc_src_ret_coord =
	    vec2_t{0.0, 0.0}
	    + 0.5 * vec2_t{static_cast<double>(ret.width()), static_cast<double>(ret.height())};

	auto const aabb =
	    0.5
	    * axisAlignedBoundingBox(
	        r * vec2_t{static_cast<double>(src.width()), static_cast<double>(src.height())},
	        Angle{0});
	auto const begin_coords = loc_src_ret_coord - aabb;
	auto const end_coords   = loc_src_ret_coord + aabb;

	for(int row = static_cast<int>(begin_coords[1]); row < static_cast<int>(end_coords[1]); ++row)
	{
		for(int col = static_cast<int>(begin_coords[0]); col < static_cast<int>(end_coords[0]);
		    ++col)
		{
			auto const loc_ret = vec2_t{static_cast<double>(col), static_cast<double>(row)};
			auto const src_pos = scale_factor * (loc_ret - loc_src_ret_coord) + origin_src;
			if(src_pos[0] >= 0 && src_pos[0] < src.width() && src_pos[1] >= 0
			   && src_pos[1] < src.height())
			{
				auto const x_0 = static_cast<uint32_t>(src_pos[0]);
				auto const y_0 = static_cast<uint32_t>(src_pos[1]);
				auto const x_1 = (x_0 + src.width() + 1) % src.width();
				auto const y_1 = (y_0 + src.height() + 1) % src.height();

				auto const z_00 = src(x_0, y_0);
				auto const z_01 = src(x_0, y_1);
				auto const z_10 = src(x_1, y_0);
				auto const z_11 = src(x_1, y_1);

				auto const coord =
				    src_pos - vec2_t{static_cast<double>(x_0), static_cast<double>(y_0)};

				auto const z_x0 = (1.0f - static_cast<float>(coord[0])) * z_00
				                  + static_cast<float>(coord[0]) * z_10;
				auto const z_x1 = (1.0f - static_cast<float>(coord[0])) * z_01
				                  + static_cast<float>(coord[0]) * z_11;
				auto const z_xy = (1.0f - static_cast<float>(coord[1])) * z_x0
				                  + static_cast<float>(coord[1]) * z_x1;

				ret((col + ret.width()) % ret.width(), (row + ret.height()) % ret.height()) = z_xy;
			}
		}
	}
}

#endif
