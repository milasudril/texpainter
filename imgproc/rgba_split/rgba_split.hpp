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
	using RetVal     = Texpainter::FilterGraph::ImgProcRetval;
	using Arg        = Texpainter::FilterGraph::ImgProcArg;
	using ParamName  = Texpainter::FilterGraph::ParamName;
	template<class T>
	using ImgProcArg = Texpainter::FilterGraph::ImgProcArg2<T>;

	class ImageProcessor
	{
		static constexpr PortInfo s_input_ports[] = {{PixelType::RGBA, "Pixels"}};

		static constexpr PortInfo s_output_ports[] = {PortInfo{PixelType::GrayscaleReal, "Red"},
		                                              PortInfo{PixelType::GrayscaleReal, "Green"},
		                                              PortInfo{PixelType::GrayscaleReal, "Blue"},
		                                              PortInfo{PixelType::GrayscaleReal, "Alpha"}};

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
			return std::span<PortInfo const>{s_input_ports};
		}

		std::vector<RetVal> operator()(std::span<Arg const>) const;

		static constexpr char const* name() { return "RGBA split"; }

	private:
	};
}

#endif
