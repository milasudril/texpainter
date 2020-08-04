//@	{
//@	 "targets":[{"name":"image_source.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_IMAGESOURCE_HPP
#define TEXPAINTER_FILTERGRAPH_IMAGESOURCE_HPP

namespace Texpainter::FilterGraph
{
	template<class PixelType>
	class ImageSource
	{
		static constexpr std::string_view s_output_port_names[] = {"Pixels"};

	public:
		explicit ImageSource(Span2d<PixelType const> pixels): r_pixels{pixels} {}

		std::span<std::string_view const> paramNames() const
		{
			return std::span<std::string_view const>{};
		}

		std::span<ProcParamValue const> paramValues() const
		{
			return std::span<ProcParamValue const>{};
		}

		void set(std::string_view, ProcParamValue) {}

		ProcParamValue get(std::string_view) const { return ProcParamValue{0.0}; }

		static constexpr std::span<std::string_view const> outputPorts()
		{
			return std::span<std::string_view const>{s_output_port_names, 1};
		}

		static constexpr std::span<std::string_view const> inputPorts()
		{
			return std::span<std::string_view const>{};
		}

		std::vector<ProcResultType> operator()(std::span<ProcArgumentType const>) const
		{
			return std::vector<ProcResultType>{PixelStore::BasicImage<PixelType>{r_pixels}};
		}

		static constexpr std::string_view name() { return "Layer input"; }

	private:
		Span2d<PixelType const> r_pixels;
	};
}

#endif
