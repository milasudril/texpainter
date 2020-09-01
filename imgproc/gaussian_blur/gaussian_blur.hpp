//@	{
//@	 "targets":[{"name":"gaussian_blur.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"gaussian_blur.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_GAUSSIANBLUR_GAUSSIANBLUR_HPP
#define TEXPAINTER_IMGPROC_GAUSSIANBLUR_GAUSSIANBLUR_HPP

#include "filtergraph/proctypes.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"

namespace GaussianBlur
{
	using PortInfo   = Texpainter::FilterGraph::PortInfo;
	using PixelType  = Texpainter::FilterGraph::PixelType;
	using ParamValue = Texpainter::FilterGraph::ParamValue;
	using ParamName  = Texpainter::FilterGraph::ParamName;

	template<class T>
	using ImgProcArg = Texpainter::FilterGraph::ImgProcArg<T>;

	using ComplexValue     = Texpainter::FilterGraph::RgbaValue;
	using RealValue        = Texpainter::FilterGraph::RealValue;
	using ImageProcessorId = Texpainter::FilterGraph::ImageProcessorId;

	class ImageProcessor
	{
	public:
		struct InterfaceDescriptor
		{
			static constexpr std::array<PortInfo, 1> InputPorts{
			    {PortInfo{PixelType::GrayscaleComplex, "Image spectrum"}}};
			static constexpr std::array<PortInfo, 1> OutputPorts{
			    {PortInfo{PixelType::GrayscaleComplex, "Image spectrum"}}};

			static constexpr std::array<ParamName, 3> ParamNames{"ξ_0", "η_0", "θ"};
		};

		void operator()(ImgProcArg<InterfaceDescriptor> const& args) const;

		void set(ParamName, ParamValue) {}

		ParamValue get(ParamName) const { return ParamValue{0.0}; }

		std::span<ParamValue const> paramValues() const { return std::span<ParamValue const>{}; }

		static constexpr char const* name() { return "Gaussian blur"; }

		static constexpr auto id() { return ImageProcessorId{"2f1a5a2f195dde21a8629191e66fdb2d"}; }

	private:
	};
}

#endif
