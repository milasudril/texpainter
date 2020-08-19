//@	{
//@	 "targets":[{"name":"image_source.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_IMAGESOURCE_HPP
#define TEXPAINTER_FILTERGRAPH_IMAGESOURCE_HPP

#include "./proctypes.hpp"

namespace Texpainter::FilterGraph
{
	namespace detail
	{
		template<class T>
		struct MapPixelType;

		template<>
		struct MapPixelType<PixelStore::Pixel>
		{
			static constexpr auto value = PixelType::RGBA;
		};
	}

	template<class PixelType>
	class ImageSource
	{
		static constexpr PortInfo s_output_ports[] = {
		    PortInfo{detail::MapPixelType<PixelType>::value, "Pixels"}};

	public:
		struct InterfaceDescriptor
		{
			static constexpr std::array<PortInfo, 0> InputPorts{};
			static constexpr std::array<PortInfo, 1> OutputPorts{
			    {{typeToPixelType<PixelType>(), "Pixels"}}};
			static constexpr std::array<ParamName, 0> ParamNames{};
		};

		void operator()(auto const& args) const
		{
			std::ranges::copy(r_pixels, args.template output<0>());
		}

		ParamValue get(ParamName) const { return ParamValue{0.0}; }

		void set(ParamName, ParamValue) {}


		explicit ImageSource(Span2d<PixelType const> pixels): r_pixels{pixels} {}

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

		std::vector<ImgProcRetval> operator()(std::span<ImgProcArg const>) const
		{
			return std::vector<ImgProcRetval>{PixelStore::BasicImage<PixelType>{r_pixels}};
		}


		static constexpr char const* name() { return "Layer input"; }

	private:
		Span2d<PixelType const> r_pixels;
	};
}

#endif
