//@	{
//@	 "targets":[{"name":"proctypes.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_PROCTYPES_HPP
#define TEXPAINTER_FILTERGRAPH_PROCTYPES_HPP

#include "pixel_store/image.hpp"

#include <compare>
#include <cstddef>
#include <variant>
#include <string_view>
#include <span>

namespace Texpainter::FilterGraph
{
	class ParamValue
	{
	public:
		explicit constexpr ParamValue(double val): m_value{val} {}

		constexpr auto operator<=>(ParamValue const&) const = default;

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

	using RealValue    = double;
	using ComplexValue = std::complex<RealValue>;

	template<PixelType id>
	struct PixelTypeToType;

	template<>
	struct PixelTypeToType<PixelType::RGBA>
	{
		using type = PixelStore::Pixel;
	};

	template<>
	struct PixelTypeToType<PixelType::GrayscaleReal>
	{
		using type = RealValue;
	};

	template<>
	struct PixelTypeToType<PixelType::GrayscaleComplex>
	{
		using type = ComplexValue;
	};


	struct PortInfo
	{
		PixelType type;
		char const* name;
	};

	using ImgProcArg = std::variant<Span2d<PixelStore::Pixel const>,
	                                Span2d<double const>,
	                                Span2d<std::complex<double> const>>;

	inline PixelType pixelType(ImgProcArg const& x) { return static_cast<PixelType>(x.index()); }

	using ImgProcRetval = std::variant<PixelStore::BasicImage<PixelStore::Pixel>,
	                                   PixelStore::BasicImage<double>,
	                                   PixelStore::BasicImage<std::complex<double>>>;

	inline PixelType pixelType(ImgProcRetval const& x) { return static_cast<PixelType>(x.index()); }

	template<class T>
	concept ImageProcessor = requires(T a)
	{
		{
			T::name()
		}
		->std::same_as<char const*>;

		{
			T::paramNames()
		}
		->std::same_as<std::span<char const* const>>;

		{
			T::inputPorts()
		}
		->std::same_as<std::span<PortInfo const>>;

		{
			T::outputPorts()
		}
		->std::same_as<std::span<PortInfo const>>;

		{
			std::as_const(a).paramValues()
		}
		->std::same_as<std::span<ParamValue const>>;

		{a.set(std::declval<std::string_view>(), std::declval<ParamValue>())};
		{
			std::as_const(a).get(std::declval<std::string_view>())
		}
		->std::same_as<ParamValue>;

		{
			std::as_const(a)(std::declval<std::span<ImgProcArg const>>())
		}
		->std::same_as<std::vector<ImgProcRetval>>;
	};
}

#endif