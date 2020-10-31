//@	{
//@	 "targets":[{"name":"layer_input.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_LAYERINPUT_HPP
#define TEXPAINTER_FILTERGRAPH_LAYERINPUT_HPP

#include "./port_info.hpp"
#include "./img_proc_arg.hpp"
#include "./image_processor_id.hpp"

#include "utils/span_2d.hpp"

namespace Texpainter::FilterGraph
{

	class LayerInput
	{
	public:
		struct InterfaceDescriptor
		{
			static constexpr std::array<PortInfo, 0> InputPorts{};
			static constexpr std::array<PortInfo, 2> OutputPorts{
			    {{PortType::RgbaPixels, "Pixels"}, {PortType::Palette, "Palette"}}};
			static constexpr std::array<ParamName, 0> ParamNames{};
		};

		void operator()(ImgProcArg<InterfaceDescriptor> const& args) const
		{
			std::ranges::copy(r_pixels, args.template output<0>());
			args.template output<1>().get() = *r_palette;
		}

		ParamValue get(ParamName) const { return ParamValue{0.0}; }

		void set(ParamName, ParamValue) {}

		std::span<ParamValue const> paramValues() const { return std::span<ParamValue const>{}; }

		static constexpr char const* name() { return "Layer input"; }

		static constexpr auto id() { return ImageProcessorId{"66bc12285d8881c91ff74fcde23e992f"}; }

		LayerInput& pixels(Span2d<RgbaValue const> src)
		{
			r_pixels = src;
			return *this;
		}

		LayerInput& palette(std::reference_wrapper<Palette const> palette)
		{
			r_palette = &palette.get();
			return *this;
		}

	private:
		Span2d<RgbaValue const> r_pixels;
		Palette const* r_palette;
	};
}

#endif
