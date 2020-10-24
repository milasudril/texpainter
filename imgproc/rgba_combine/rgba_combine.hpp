//@	{
//@	 "targets":[{"name":"rgba_combine.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"rgba_combine.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_RGBACOMBINE_RGBACOMBINE_HPP
#define TEXPAINTER_IMGPROC_RGBACOMBINE_RGBACOMBINE_HPP

#include "filtergraph/port_info.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"

namespace RgbaCombine
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
			static constexpr std::array<PortInfo, 4> InputPorts{
			    {PortInfo{PortType::GrayscaleRealPixels, "Red"},
			     PortInfo{PortType::GrayscaleRealPixels, "Green"},
			     PortInfo{PortType::GrayscaleRealPixels, "Blue"},
			     PortInfo{PortType::GrayscaleRealPixels, "Alpha"}}};
			static constexpr std::array<PortInfo, 1> OutputPorts{{PortType::RGBAPixels, "Pixels"}};

			static constexpr std::array<ParamName, 0> ParamNames{};
		};

		void operator()(ImgProcArg<InterfaceDescriptor> const& args) const;

		void set(ParamName, ParamValue) {}

		ParamValue get(ParamName) const { return ParamValue{0.0}; }

		std::span<ParamValue const> paramValues() const { return std::span<ParamValue const>{}; }

		static constexpr char const* name() { return "RGBA combine"; }

		static constexpr auto id() { return ImageProcessorId{"2f1a5a2f195dde21a8629191e66fdb2d"}; }

	private:
	};
}

#endif
