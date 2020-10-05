//@	{
//@	 "targets":[{"name":"complex_real_multiply.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_REALREALMULTIPLY_REALREALMULTIPLY_HPP
#define TEXPAINTER_IMGPROC_REALREALMULTIPLY_REALREALMULTIPLY_HPP

#include "filtergraph/proctypes.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"

#include <algorithm>

namespace RealRealMultiply
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
			    {PortInfo{PixelType::GrayscaleReal, "Input A"},
			     PortInfo{PixelType::GrayscaleReal, "Input B"}}};
			static constexpr std::array<PortInfo, 1> OutputPorts{
			    {PixelType::GrayscaleReal, "Output"}};

			static constexpr std::array<ParamName, 0> ParamNames{};
		};

		void operator()(ImgProcArg<InterfaceDescriptor> const& args) const
		{
			auto const size = args.size().area();
			std::transform(args.input<0>(),
			               args.input<0>() + size,
			               args.input<1>(),
			               args.output<0>(),
			               std::multiplies{});
		}

		void set(ParamName, ParamValue) {}

		ParamValue get(ParamName) const { return ParamValue{0.0}; }

		std::span<ParamValue const> paramValues() const { return std::span<ParamValue const>{}; }

		static constexpr char const* name() { return "Real × Real"; }

		static constexpr auto id() { return ImageProcessorId{"487b4182c3b147b1fa04162cf9c17723"}; }

	private:
	};
}

#endif
