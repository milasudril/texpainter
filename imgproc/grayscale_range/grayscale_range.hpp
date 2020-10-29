//@	{
//@	 "targets":[{"name":"grayscale_range.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"grayscale_range.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_GRAYSCALERANGE_GRAYSCALERANGE_HPP
#define TEXPAINTER_IMGPROC_GRAYSCALERANGE_GRAYSCALERANGE_HPP

#include "filtergraph/port_info.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"
#include "filtergraph/param_map.hpp"

namespace GrayscaleRange
{
	using Texpainter::Size2d;
	using Texpainter::FilterGraph::ImageProcessorId;
	using Texpainter::FilterGraph::ImgProcArg;
	using Texpainter::FilterGraph::ParamMap;
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
			static constexpr std::array<PortInfo, 1> InputPorts{
			    PortInfo{PortType::GrayscaleRealPixels, "Input"}};
			static constexpr std::array<PortInfo, 2> OutputPorts{
			    {PortInfo{PortType::RealValue, "Min"}, PortInfo{PortType::RealValue, "Max"}}};

			static constexpr std::array<ParamName, 0> ParamNames{};
		};

		void operator()(ImgProcArg<InterfaceDescriptor> const& args) const;

		void set(ParamName, ParamValue) {}

		ParamValue get(ParamName) const { return ParamValue{0.0}; }

		std::span<ParamValue const> paramValues() const { return std::span<ParamValue const>{}; }

		static constexpr char const* name() { return "Grayscale range"; }

		static constexpr auto id() { return ImageProcessorId{"b8ffe044a428a6c79faeba93ce715bdb"}; }

	private:
		ParamMap<InterfaceDescriptor> m_params;
	};
}

#endif
