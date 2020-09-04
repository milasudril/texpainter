//@	{
//@	 "targets":[{"name":"complex_real_multiply.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"complex_real_multiply.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_COMPLEXREALMULTIPLY_COMPLEXREALMULTIPLY_HPP
#define TEXPAINTER_IMGPROC_COMPLEXREALMULTIPLY_COMPLEXREALMULTIPLY_HPP

#include "filtergraph/proctypes.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"

namespace ComplexRealMultiply
{
	using Texpainter::FilterGraph::ComplexValue;
	using Texpainter::FilterGraph::ImageProcessorId;
	using Texpainter::FilterGraph::ImgProcArg;
	using Texpainter::FilterGraph::ParamName;
	using Texpainter::FilterGraph::ParamValue;
	using Texpainter::FilterGraph::PixelType;
	using Texpainter::FilterGraph::PortInfo;
	using Texpainter::FilterGraph::RealValue;

	class ImageProcessor
	{
	public:
		struct InterfaceDescriptor
		{
			static constexpr std::array<PortInfo, 2> InputPorts{
			    {PortInfo{PixelType::GrayscaleComplex, "Input"},
			     PortInfo{PixelType::GrayscaleReal, "Input"}}};
			static constexpr std::array<PortInfo, 1> OutputPorts{
			    {PixelType::GrayscaleComplex, "Output"}};

			static constexpr std::array<ParamName, 0> ParamNames{};
		};

		void operator()(ImgProcArg<InterfaceDescriptor> const& args) const;

		void set(ParamName, ParamValue) {}

		ParamValue get(ParamName) const { return ParamValue{0.0}; }

		std::span<ParamValue const> paramValues() const { return std::span<ParamValue const>{}; }

		static constexpr char const* name() { return "Complex × Real"; }

		static constexpr auto id() { return ImageProcessorId{"b50bf360be9b511bf8bc6129956f0158"}; }

	private:
	};
}

#endif
