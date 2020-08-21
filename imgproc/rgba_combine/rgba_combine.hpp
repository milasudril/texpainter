//@	{
//@	 "targets":[{"name":"rgba_combine.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"rgba_combine.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_RGBACOMBINE_RGBACOMBINE_HPP
#define TEXPAINTER_IMGPROC_RGBACOMBINE_RGBACOMBINE_HPP

#include "filtergraph/proctypes.hpp"
#include "filtergraph/img_proc_arg.hpp"

namespace RgbaCombine
{
	using PortInfo   = Texpainter::FilterGraph::PortInfo;
	using PixelType  = Texpainter::FilterGraph::PixelType;
	using ParamValue = Texpainter::FilterGraph::ParamValue;
	using ParamName  = Texpainter::FilterGraph::ParamName;
	template<class T>
	using ImgProcArg = Texpainter::FilterGraph::ImgProcArg2<T>;
	using RgbaValue  = Texpainter::FilterGraph::RgbaValue;
	using RealValue  = Texpainter::FilterGraph::RealValue;

	class ImageProcessor
	{
	public:
		struct InterfaceDescriptor
		{
			static constexpr std::array<PortInfo, 4> InputPorts{
			    {PortInfo{PixelType::GrayscaleReal, "Red"},
			     PortInfo{PixelType::GrayscaleReal, "Green"},
			     PortInfo{PixelType::GrayscaleReal, "Blue"},
			     PortInfo{PixelType::GrayscaleReal, "Alpha"}}};
			static constexpr std::array<PortInfo, 1> OutputPorts{{PixelType::RGBA, "Pixels"}};

			static constexpr std::array<ParamName, 0> ParamNames{};
		};

		void operator()(ImgProcArg<InterfaceDescriptor> const& args) const;

		void set(ParamName, ParamValue) {}

		ParamValue get(ParamName) const { return ParamValue{0.0}; }

		std::span<ParamValue const> paramValues() const { return std::span<ParamValue const>{}; }

		static constexpr char const* name() { return "RGBA combine"; }

	private:
	};
}

#endif
