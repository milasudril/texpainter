//@	{
//@	 "targets":[{"name":"image_sink.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_MODEL_IMAGESINK_HPP
#define TEXPAINTER_MODEL_IMAGESINK_HPP

#include "filtergraph/port_info.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"

#include "utils/span_2d.hpp"

#include <cassert>

namespace Texpainter::Model
{
	class ImageSink
	{
	public:
		struct InterfaceDescriptor
		{
			static constexpr std::array<FilterGraph::PortInfo, 1> InputPorts{
			    {FilterGraph::PortType::RgbaPixels, "Pixels"}};
			static constexpr std::array<FilterGraph::PortInfo, 1> OutputPorts{
			    {FilterGraph::PortType::RgbaPixels, "Pixels", true}};
			static constexpr std::array<FilterGraph::ParamName, 0> ParamNames{};
		};

		void operator()(FilterGraph::ImgProcArg<InterfaceDescriptor> const& args) const
		{
			assert(r_pixels.data() != nullptr);
			std::copy_n(args.input<0>(), area(args.canvasSize()), r_pixels.data());
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

		static constexpr char const* name() { return "Output image"; }

		static constexpr auto id()
		{
			return FilterGraph::ImageProcessorId{"4332d23feb31f1daf36caf312aca0911"};
		}

		static constexpr auto releaseState() { return FilterGraph::ImgProcReleaseState::Stable; }

		void sink(Span2d<FilterGraph::RgbaValue> val) { r_pixels = val; }

	private:
		Span2d<FilterGraph::RgbaValue> r_pixels;
	};
}

#endif
