//@	{
//@	 "targets":[{"name":"grayscale_noise.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"grayscale_noise.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_GRAYSCALENOISE_GRAYSCALENOISE_HPP
#define TEXPAINTER_IMGPROC_GRAYSCALENOISE_GRAYSCALENOISE_HPP

#include "filtergraph/proctypes.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"
#include "filtergraph/param_map.hpp"

namespace GrayscaleNoise
{
	using Texpainter::Size2d;
	using Texpainter::FilterGraph::ImageProcessorId;
	using Texpainter::FilterGraph::ImgProcArg;
	using Texpainter::FilterGraph::ParamMap;
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
			static constexpr std::array<PortInfo, 0> InputPorts{};
			static constexpr std::array<PortInfo, 1> OutputPorts{
			    {PortInfo{PixelType::GrayscaleReal, "Output"}}};

			static constexpr std::array<ParamName, 0> ParamNames{};
		};

		void operator()(ImgProcArg<InterfaceDescriptor> const& args) const;

		void set(ParamName, ParamValue) {}

		ParamValue get(ParamName) const { return ParamValue{0.0}; }

		std::span<ParamValue const> paramValues() const { return std::span<ParamValue const>{}; }

		static constexpr char const* name() { return "Grayscale noise"; }

		static constexpr auto id() { return ImageProcessorId{"5d03472443bfc964fea98df862b20bf6"}; }

	private:
		ParamMap<InterfaceDescriptor> m_params;
	};
}

#endif
