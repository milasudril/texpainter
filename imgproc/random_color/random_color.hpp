//@	{
//@	 "targets":[{"name":"random_color.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_RANDOMCOLOR_RANDOMCOLOR_HPP
#define TEXPAINTER_IMGPROC_RANDOMCOLOR_RANDOMCOLOR_HPP

#include "filtergraph/port_info.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"

#include <algorithm>

namespace RandomColor
{
	using Texpainter::FilterGraph::ImageProcessorId;
	using Texpainter::FilterGraph::ImgProcArg;
	using Texpainter::FilterGraph::Palette;
	using Texpainter::FilterGraph::ParamName;
	using Texpainter::FilterGraph::ParamValue;
	using Texpainter::FilterGraph::PortInfo;
	using Texpainter::FilterGraph::PortType;
	using Texpainter::FilterGraph::RealValue;

	class ImageProcessor
	{
	public:
		struct InterfaceDescriptor
		{
			static constexpr std::array<PortInfo, 2> InputPorts{
			    {PortInfo{PortType::GrayscaleRealPixels, "Random src"},
			     PortInfo{PortType::Palette, "Palette"}}};
			static constexpr std::array<PortInfo, 1> OutputPorts{{PortType::RgbaPixels, "Output"}};

			static constexpr std::array<ParamName, 0> ParamNames{};
		};

		void operator()(ImgProcArg<InterfaceDescriptor> const& args) const
		{
			auto const size = args.size().area();
			std::transform(args.input<0>(),
			               args.input<0>() + size,
			               args.output<0>(),
			               [&palette = args.input<1>().get()](auto val) {
				               auto const n_colors = palette.size();
				               auto const val_int  = Palette::index_type{
                                   std::clamp(static_cast<uint32_t>(n_colors * val),
                                              0u,
                                              static_cast<uint32_t>(n_colors - 1))};
				               return palette[val_int];
			               });
		}

		void set(ParamName, ParamValue) {}

		ParamValue get(ParamName) const { return ParamValue{0.0}; }

		std::span<ParamValue const> paramValues() const { return std::span<ParamValue const>{}; }

		static constexpr char const* name() { return "Random color"; }

		static constexpr auto id() { return ImageProcessorId{"5c10a297b5c5b898a2f5c2e265caa209"}; }

	private:
	};
}

#endif
