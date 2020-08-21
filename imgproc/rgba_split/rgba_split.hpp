//@	{
//@	 "targets":[{"name":"rgba_split.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"rgba_split.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_RGBASPLIT_RGBASPLIT_HPP
#define TEXPAINTER_IMGPROC_RGBASPLIT_RGBASPLIT_HPP

#include "filtergraph/proctypes.hpp"
#include "filtergraph/img_proc_arg.hpp"

namespace RgbaSplit
{
	using PortInfo   = Texpainter::FilterGraph::PortInfo;
	using PixelType  = Texpainter::FilterGraph::PixelType;
	using ParamValue = Texpainter::FilterGraph::ParamValue;
	using ParamName  = Texpainter::FilterGraph::ParamName;
	template<class T>
	using ImgProcArg = Texpainter::FilterGraph::ImgProcArg<T>;
	using RgbaValue  = Texpainter::FilterGraph::RgbaValue;
	using RealValue  = Texpainter::FilterGraph::RealValue;

	class ImageProcessor
	{
	public:
		struct InterfaceDescriptor
		{
			static constexpr std::array<PortInfo, 1> InputPorts{{PixelType::RGBA, "Pixels"}};

			static constexpr std::array<PortInfo, 4> OutputPorts{
			    {{PixelType::GrayscaleReal, "Red"},
			     {PixelType::GrayscaleReal, "Green"},
			     {PixelType::GrayscaleReal, "Blue"},
			     {PixelType::GrayscaleReal, "Alpha"}}};

			static constexpr std::array<ParamName, 0> ParamNames{};
		};

		void operator()(ImgProcArg<InterfaceDescriptor> const&) const;

		ParamValue get(ParamName) const { return ParamValue{0.0}; }

		void set(ParamName, ParamValue) {}

		std::span<ParamValue const> paramValues() const { return std::span<ParamValue const>{}; }

		static constexpr char const* name() { return "RGBA split"; }

	private:
	};
}

#endif
