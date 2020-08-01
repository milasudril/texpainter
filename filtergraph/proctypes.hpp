//@	{
//@	 "targets":[{"name":"proctypes.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_HPP
#define TEXPAINTER_FILTERGRAPH_HPP

#include "pixel_store/image.hpp"

#include <compare>
#include <cstddef>
#include <variant>
#include <string_view>
#include <span>

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

	using ProcArgumentType = std::variant<Span2d<PixelStore::Pixel const>,
	                                      Span2d<double const>,
	                                      Span2d<std::complex<double> const>>;

	inline PixelType pixelType(ProcArgumentType const& x)
	{
		return static_cast<PixelType>(x.index());
	}

	using ProcResultType = std::variant<PixelStore::BasicImage<PixelStore::Pixel>,
	                                    PixelStore::BasicImage<double>,
	                                    PixelStore::BasicImage<std::complex<double>>>;

	inline PixelType pixelType(ProcResultType const& x)
	{
		return static_cast<PixelType>(x.index());
	}

	template<class T>
	concept ImageProcessor = requires(T a)
	{
		{
			std::as_const(a).paramNames()
		}
		->std::same_as<std::span<std::string_view const>>;

		{
			std::as_const(a).paramValues()
		}
		->std::same_as<std::vector<ProcParamValue>>;

		{a.set(std::declval<std::string_view>(), std::declval<ProcParamValue>())};
		{
			std::as_const(a).get(std::declval<std::string_view>())
		}
		->std::same_as<ProcParamValue>;

		{
			std::as_const(a).inputPorts()
		}
		->std::same_as<std::span<std::string_view const>>;

		{
			std::as_const(a).outputPorts()
		}
		->std::same_as<std::span<std::string_view const>>;

		{
			std::as_const(a)(std::declval<std::span<ProcArgumentType const>>())
		}
		->std::same_as<std::vector<ProcResultType>>;
	};
}

#endif