//@	{
//@	 "targets":[{"name":"image_source.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_MODELNEW_IMAGESOURCE_HPP
#define TEXPAINTER_MODELNEW_IMAGESOURCE_HPP

#include "filtergraph/port_info.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"

#include "utils/span_2d.hpp"

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

		void operator()(FilterGraph::ImgProcArg<InterfaceDescriptor> const& args) const
		{
			std::ranges::copy(r_pixels, args.template output<0>());
		}

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

		static constexpr auto id()
		{
			return FilterGraph::ImageProcessorId{"66bc12285d8881c91ff74fcde23e992f"};
		}

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

#endif
