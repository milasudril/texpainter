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
	class ProcParamValue
	{
	public:
		explicit constexpr ProcParamValue(double val): m_value{val} {}

		constexpr auto operator<=>(ProcParamValue const&) const = default;

		constexpr double value() const { return m_value; }

	private:
		double m_value;
	};

	enum class PortType : int
	{
		Input,
		Output
	};

	template<PortType type>
	class Port
	{
	public:
		explicit constexpr Port(uint32_t val): m_value{val} {}

		constexpr uint32_t value() const { return m_value; }

	private:
		uint32_t m_value;
	};

	template<PortType type>
	constexpr bool operator==(Port<type> a, Port<type> b)
	{
		return a.value() == b.value();
	}

	template<PortType type>
	constexpr bool operator!=(Port<type> a, Port<type> b)
	{
		return !(a == b);
	}

	using InputPort  = Port<PortType::Input>;
	using OutputPort = Port<PortType::Output>;


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