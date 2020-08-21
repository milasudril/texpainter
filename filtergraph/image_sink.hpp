//@	{
//@	 "targets":[{"name":"image_sink.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_IMAGESINK_HPP
#define TEXPAINTER_FILTERGRAPH_IMAGESINK_HPP

#include "./proctypes.hpp"
#include "./img_proc_arg.hpp"

#include <vector>
#include <cassert>

namespace Texpainter::FilterGraph
{
	class ImageSink
	{
		static constexpr PortInfo s_input_ports[] = {{PixelType::RGBA, "Pixels"}};

	public:
		struct InterfaceDescriptor
		{
			static constexpr std::array<PortInfo, 1> InputPorts{{PixelType::RGBA, "Pixels"}};
			static constexpr std::array<PortInfo, 0> OutputPorts{};
			static constexpr std::array<ParamName, 0> ParamNames{};
		};

		void operator()(ImgProcArg2<InterfaceDescriptor> const& args) const
		{
			assert(r_pixels.data() != nullptr);
			std::copy_n(args.input<0>(), args.size().area(), r_pixels.data());
		}

		ParamValue get(ParamName) const { return ParamValue{0.0}; }

		void set(ParamName, ParamValue) {}


		static constexpr std::span<char const* const> paramNames()
		{
			return std::span<char const* const>{};
		}

		std::span<ParamValue const> paramValues() const { return std::span<ParamValue const>{}; }

		void set(std::string_view, ParamValue) {}

		void pixels(Span2d<RgbaValue> val) { r_pixels = val; }

		ParamValue get(std::string_view) const { return ParamValue{0.0}; }

		static constexpr std::span<PortInfo const> inputPorts()
		{
			return std::span<PortInfo const>{s_input_ports};
		}

		static constexpr std::span<PortInfo const> outputPorts()
		{
			return std::span<PortInfo const>{};
		}

		std::vector<ImgProcRetval> operator()(std::span<ImgProcArg const> args) const
		{
			if(args.size() != 1) [[unlikely]]
				{
					return std::vector<ImgProcRetval>{};
				}

			if(auto args_0 = std::get_if<Span2d<PixelStore::Pixel const>>(&args[0]);
			   args_0 != nullptr)
				[[likely]] { return std::vector<ImgProcRetval>{PixelStore::Image{*args_0}}; }

			return std::vector<ImgProcRetval>{};
		}

		static constexpr char const* name() { return "Layer output"; }

	private:
		Span2d<RgbaValue> r_pixels;
	};
}

#endif
