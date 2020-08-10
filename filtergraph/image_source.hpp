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
		explicit ImageSource(Span2d<PixelType const> pixels): r_pixels{pixels} {}

		static constexpr std::span<char const* const> paramNames()
		{
			return std::span<char const* const>{};
		}

		std::span<ProcParamValue const> paramValues() const
		{
			return std::span<ProcParamValue const>{};
		}

		void set(std::string_view, ProcParamValue) {}

		ProcParamValue get(std::string_view) const { return ProcParamValue{0.0}; }

		static constexpr std::span<PortInfo const> outputPorts()
		{
			return std::span<PortInfo const>{s_output_ports};
		}

		static constexpr std::span<PortInfo const> inputPorts()
		{
			return std::span<PortInfo const>{};
		}

		std::vector<ProcResultType> operator()(std::span<ProcArgumentType const>) const
		{
			return std::vector<ProcResultType>{PixelStore::BasicImage<PixelType>{r_pixels}};
		}

		static constexpr char const* name() { return "Layer input"; }

	private:
		Span2d<PixelType const> r_pixels;
	};
}

#endif
