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
#include <ranges>

namespace Texpainter::FilterGraph
{
	class ParamValue
	{
	public:
		explicit constexpr ParamValue() = default;

		explicit constexpr ParamValue(double val): m_value{val} {}

		constexpr auto operator<=>(ParamValue const&) const = default;

		constexpr double value() const { return m_value; }

	private:
		double m_value;
	};

	class ParamName
	{
	public:
		constexpr ParamName(char const* str): r_str{str} {}

		constexpr char const* c_str() const { return r_str; }

		constexpr auto operator<=>(ParamName other) const
		{
			auto res = strcmp(r_str, other.r_str);
			if(res == 0) [[unlikely]]
				{
					return std::strong_ordering::equal;
				}

			if(res < 0) { return std::strong_ordering::less; }

			return std::strong_ordering::greater;
		}

		constexpr bool operator==(ParamName const& other) const = default;

	private:
		char const* r_str;
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

	using RgbaValue    = Texpainter::PixelStore::Pixel;
	using RealValue    = double;
	using ComplexValue = std::complex<RealValue>;

	template<PixelType id>
	struct PixelTypeToType;

	template<>
	struct PixelTypeToType<PixelType::RGBA>
	{
		using type = RgbaValue;
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

	template<class T>
	constexpr PixelType typeToPixelType() = delete;

	template<>
	constexpr PixelType typeToPixelType<RgbaValue>()
	{
		return PixelType::RGBA;
	}

	template<>
	constexpr PixelType typeToPixelType<RealValue>()
	{
		return PixelType::GrayscaleReal;
	}

	template<>
	constexpr PixelType typeToPixelType<ComplexValue>()
	{
		return PixelType::GrayscaleComplex;
	}


	struct PortInfo
	{
		PixelType type;
		char const* name;
	};

	constexpr bool operator==(PortInfo a, PortInfo b)
	{
		if(a.type == b.type) { return strcmp(a.name, b.name) == 0; }
		return false;
	}

	constexpr bool operator!=(PortInfo a, PortInfo b) { return !(a == b); }
}

#endif