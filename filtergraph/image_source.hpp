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

		std::span<std::string_view const> outputPorts() const
		{
			constexpr std::string_view ret[] = {"Output"};
			return std::span<std::string_view const>{ret, 1};
		}

		std::span<std::string_view const> inputPorts() const
		{
			return std::span<std::string_view const>{};
		}

		std::vector<ProcResultType> operator()(std::span<ProcArgumentType const>) const
		{
			return std::vector<ProcResultType>{PixelStore::BasicImage<PixelType>{r_pixels}};
		}

		static constexpr std::string_view name()
		{
			return "Layer input";
		}

	private:
		Span2d<PixelType const> r_pixels;
	};
}

#endif
