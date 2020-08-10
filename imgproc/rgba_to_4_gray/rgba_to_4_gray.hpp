//@	{
//@	 "targets":[{"name":"rgba_to_4_gray.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"rgba_to_4_gray.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_RGBATO4GRAY_RGBATO4GRAY_HPP
#define TEXPAINTER_IMGPROC_RGBATO4GRAY_RGBATO4GRAY_HPP

#include "filtergraph/proctypes.hpp"

namespace RgbaTo4Gray
{
	using PortInfo   = Texpainter::FilterGraph::PortInfo;
	using PixelType  = Texpainter::FilterGraph::PixelType;
	using ParamValue = Texpainter::FilterGraph::ParamValue;
	using RetVal     = Texpainter::FilterGraph::ImgProcRetval;
	using Arg        = Texpainter::FilterGraph::ImgProcArg;

	class ImageProcessor
	{
		static constexpr PortInfo s_input_ports[] = {PixelType::RGBA, "Red"};

		static constexpr PortInfo s_output_ports[] = {PortInfo{PixelType::GrayscaleReal, "Red"},
		                                              PortInfo{PixelType::GrayscaleReal, "Green"},
		                                              PortInfo{PixelType::GrayscaleReal, "Blue"},
		                                              PortInfo{PixelType::GrayscaleReal, "Alpha"}};

	public:
		static constexpr std::span<char const* const> paramNames()
		{
			return std::span<char const* const>{};
		}

		std::span<ParamValue const> paramValues() const { return std::span<ParamValue const>{}; }

		void set(std::string_view, ParamValue) {}

		ParamValue get(std::string_view) const { return ParamValue{0.0}; }

		static constexpr std::span<PortInfo const> outputPorts()
		{
			return std::span<PortInfo const>{s_output_ports};
		}

		static constexpr std::span<PortInfo const> inputPorts()
		{
			return std::span<PortInfo const>{};
		}

		std::vector<RetVal> operator()(std::span<Arg const>) const;

		static constexpr char const* name() { return "RGBA split"; }

	private:
	};
}

#endif
