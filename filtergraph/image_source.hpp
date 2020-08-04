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
		static constexpr char const* s_output_port_names[] = {"Pixels"};

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

		static constexpr std::span<char const* const> outputPorts()
		{
			return std::span<char const* const>{s_output_port_names};
		}

		static constexpr std::span<char const* const> inputPorts()
		{
			return std::span<char const* const>{};
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
