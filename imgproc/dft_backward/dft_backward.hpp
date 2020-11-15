//@	{
//@	 "targets":[{"name":"dft_backward.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"dft_backward.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_DFTBACKWARD_DFTBACKWARD_HPP
#define TEXPAINTER_IMGPROC_DFTBACKWARD_DFTBACKWARD_HPP

#include "filtergraph/port_info.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"

namespace DftBackward
{
	using Texpainter::FilterGraph::ComplexValue;
	using Texpainter::FilterGraph::ImageProcessorId;
	using Texpainter::FilterGraph::ImgProcArg;
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
			    {PortInfo{PortType::GrayscaleComplexPixels, "Image spectrum"}}};
			static constexpr std::array<PortInfo, 1> OutputPorts{
			    {PortType::GrayscaleRealPixels, "Pixels"}};

			static constexpr std::array<ParamName, 0> ParamNames{};
		};

		void operator()(ImgProcArg<InterfaceDescriptor> const& args) const;

		void set(ParamName, ParamValue) {}

		ParamValue get(ParamName) const { return ParamValue{0.0}; }

		std::span<ParamValue const> paramValues() const { return std::span<ParamValue const>{}; }

		static constexpr char const* name() { return "DFT Backward"; }

		static constexpr auto id() { return ImageProcessorId{"E4CA00ACA31445D47FB510B3102C9529"}; }

	private:
	};
}

#endif
