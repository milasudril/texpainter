//@	{
//@	 "targets":[{"name":"normalize_grayscale.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"normalize_grayscale.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_NORMALIZEGRAYSCALE_NORMALIZEGRAYSCALE_HPP
#define TEXPAINTER_IMGPROC_NORMALIZEGRAYSCALE_NORMALIZEGRAYSCALE_HPP

#include "filtergraph/proctypes.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"
#include "filtergraph/param_map.hpp"

namespace NormalizeGrayscale
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
			static constexpr std::array<PortInfo, 1> OutputPorts{
			    {PortInfo{PortType::GrayscaleRealPixels, "Output"}}};

			static constexpr std::array<ParamName, 0> ParamNames{};
		};

		void operator()(ImgProcArg<InterfaceDescriptor> const& args) const;

		void set(ParamName, ParamValue) {}

		ParamValue get(ParamName) const { return ParamValue{0.0}; }

		std::span<ParamValue const> paramValues() const { return std::span<ParamValue const>{}; }

		static constexpr char const* name() { return "Normalize grayscale"; }

		static constexpr auto id() { return ImageProcessorId{"15cdd6a6bc1a1c9fa1722b6f9150b9de"}; }

	private:
		ParamMap<InterfaceDescriptor> m_params;
	};
}

#endif
