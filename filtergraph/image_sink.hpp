//@	{
//@	 "targets":[{"name":"image_sink.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_IMAGESINK_HPP
#define TEXPAINTER_FILTERGRAPH_IMAGESINK_HPP

#include "./proctypes.hpp"
#include "./img_proc_arg.hpp"
#include "./image_processor_id.hpp"

#include <cassert>

namespace Texpainter::FilterGraph
{
	class ImageSink
	{
	public:
		struct InterfaceDescriptor
		{
			static constexpr std::array<PortInfo, 1> InputPorts{{PixelType::RGBA, "Pixels"}};
			static constexpr std::array<PortInfo, 0> OutputPorts{};
			static constexpr std::array<ParamName, 0> ParamNames{};
		};

		void operator()(ImgProcArg<InterfaceDescriptor> const& args) const
		{
			assert(r_pixels.data() != nullptr);
			std::copy_n(args.input<0>(), args.size().area(), r_pixels.data());
		}

		ParamValue get(ParamName) const { return ParamValue{0.0}; }

		void set(ParamName, ParamValue) {}

		std::span<ParamValue const> paramValues() const { return std::span<ParamValue const>{}; }

		static constexpr char const* name() { return "Layer output"; }

		static constexpr auto id() { return ImageProcessorId{"4332d23feb31f1daf36caf312aca0911"}; }

		void sink(Span2d<RgbaValue> val) { r_pixels = val; }

	private:
		Span2d<RgbaValue> r_pixels;
	};
}

#endif
