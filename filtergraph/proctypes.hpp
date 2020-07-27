//@	{
//@	 "targets":[{"name":"proctypes.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_HPP
#define TEXPAINTER_FILTERGRAPH_HPP

#include "model/image.hpp"

#include <compare>
#include <cstddef>
#include <variant>

namespace Texpainter::FilterGraph
{
	struct ProcParamValue
	{
		double m_value;

		constexpr auto operator<=>(ProcParamValue const&) const = default;
	};


	enum class PixelType : size_t
	{
		RGBA,
		GrayscaleReal,
		GrayscaleComplex
	};

	using ProcArgumentType = std::variant<Span2d<Model::Pixel const>,
	                                      Span2d<double const>,
	                                      Span2d<std::complex<double> const>>;

	inline PixelType pixelType(ProcArgumentType const& x)
	{
		return static_cast<PixelType>(x.index());
	}

	using ProcResultType = std::variant<Model::BasicImage<Model::Pixel>,
	                                    Model::BasicImage<double>,
	                                    Model::BasicImage<std::complex<double>>>;

	inline PixelType pixelType(ProcResultType const& x)
	{
		return static_cast<PixelType>(x.index());
	}
}

#endif