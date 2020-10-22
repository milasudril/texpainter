//@	{
//@	 "targets":[{"name":"rgba_split.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"rgba_split.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_RGBASPLIT_RGBASPLIT_HPP
#define TEXPAINTER_IMGPROC_RGBASPLIT_RGBASPLIT_HPP

#include "filtergraph/proctypes.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"

namespace RgbaSplit
{
	using Texpainter::FilterGraph::ImageProcessorId;
	using Texpainter::FilterGraph::ImgProcArg;
	using Texpainter::FilterGraph::ParamName;
	using Texpainter::FilterGraph::ParamValue;
	using Texpainter::FilterGraph::PortInfo;
	using Texpainter::FilterGraph::PortType;
	using Texpainter::FilterGraph::RealValue;
	using Texpainter::FilterGraph::RgbaValue;

	class ImageProcessor
	{
	public:
		struct InterfaceDescriptor
		{
			static constexpr std::array<PortInfo, 1> InputPorts{{PortType::RGBAPixels, "Pixels"}};

			static constexpr std::array<PortInfo, 4> OutputPorts{
			    {{PortType::GrayscaleRealPixels, "Red"},
			     {PortType::GrayscaleRealPixels, "Green"},
			     {PortType::GrayscaleRealPixels, "Blue"},
			     {PortType::GrayscaleRealPixels, "Alpha"}}};

			static constexpr std::array<ParamName, 0> ParamNames{};
		};

		void operator()(ImgProcArg<InterfaceDescriptor> const&) const;

		ParamValue get(ParamName) const { return ParamValue{0.0}; }

		void set(ParamName, ParamValue) {}

		std::span<ParamValue const> paramValues() const { return std::span<ParamValue const>{}; }

		static constexpr char const* name() { return "RGBA split"; }

		static constexpr auto id() { return ImageProcessorId{"369c906fb7cfe8a1e10f5e8012ab5e60"}; }

	private:
	};
}

#endif
