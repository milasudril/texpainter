//@	{
//@	 "targets":[{"name":"dft_forward.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"dft_forward.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_DFTFORWARD_DFTFORWARD_HPP
#define TEXPAINTER_IMGPROC_DFTFORWARD_DFTFORWARD_HPP

#include "filtergraph/proctypes.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"

#include "dft/engine.hpp"

namespace DftForward
{
	using Texpainter::FilterGraph::ComplexValue;
	using Texpainter::FilterGraph::ImageProcessorId;
	using Texpainter::FilterGraph::ImgProcArg;
	using Texpainter::FilterGraph::ParamName;
	using Texpainter::FilterGraph::ParamValue;
	using Texpainter::FilterGraph::PixelType;
	using Texpainter::FilterGraph::PortInfo;
	using Texpainter::FilterGraph::RealValue;

	namespace Dft = Texpainter::Dft;

	class ImageProcessor
	{
	public:
		ImageProcessor(): r_dft_engine{&Dft::engineInstance()} {};

		struct InterfaceDescriptor
		{
			static constexpr std::array<PortInfo, 1> InputPorts{
			    {PortInfo{PixelType::GrayscaleReal, "Pixels"}}};
			static constexpr std::array<PortInfo, 1> OutputPorts{
			    {PixelType::GrayscaleComplex, "Image spectrum"}};

			static constexpr std::array<ParamName, 0> ParamNames{};
		};

		void operator()(ImgProcArg<InterfaceDescriptor> const& args) const;

		void set(ParamName, ParamValue) {}

		ParamValue get(ParamName) const { return ParamValue{0.0}; }

		std::span<ParamValue const> paramValues() const { return std::span<ParamValue const>{}; }

		static constexpr char const* name() { return "DFT Forward"; }

		static constexpr auto id() { return ImageProcessorId{"3B0189D5F690713FF7D02B05E74571D1"}; }

	private:
		Dft::Engine const* r_dft_engine;
	};
}

#endif
