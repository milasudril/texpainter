//@	{
//@	 "targets":[{"name":"proctypes.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_PROCTYPES_HPP
#define TEXPAINTER_FILTERGRAPH_PROCTYPES_HPP

#include "pixel_store/image.hpp"
#include "libenum/variant.hpp"

#include <compare>
#include <cstddef>
#include <variant>
#include <string_view>
#include <span>
#include <ranges>

namespace Texpainter::FilterGraph
{
	enum class PortDirection : int
	{
		Input,
		Output
	};

	template<PortDirection dir>
	class PortIndex
	{
	public:
		explicit constexpr PortIndex(uint32_t val): m_value{val} {}

		constexpr uint32_t value() const { return m_value; }

	private:
		uint32_t m_value;
	};

	template<PortDirection dir>
	constexpr bool operator==(PortIndex<dir> a, PortIndex<dir> b)
	{
		return a.value() == b.value();
	}

	template<PortDirection dir>
	constexpr bool operator!=(PortIndex<dir> a, PortIndex<dir> b)
	{
		return !(a == b);
	}

	using InputPortIndex  = PortIndex<PortDirection::Input>;
	using OutputPortIndex = PortIndex<PortDirection::Output>;


	enum class PortType : size_t
	{
		RGBAPixels,
		GrayscaleRealPixels,
		GrayscaleComplexPixels
	};

	using RgbaValue    = Texpainter::PixelStore::Pixel;
	using RealValue    = double;
	using ComplexValue = std::complex<RealValue>;

	template<PortType id>
	struct PortTypeToType;

	template<>
	struct PortTypeToType<PortType::RGBAPixels>
	{
		using type = RgbaValue;
	};

	template<>
	struct PortTypeToType<PortType::GrayscaleRealPixels>
	{
		using type = RealValue;
	};

	template<>
	struct PortTypeToType<PortType::GrayscaleComplexPixels>
	{
		using type = ComplexValue;
	};

	template<class T>
	constexpr PortType typeToPortType() = delete;

	template<>
	constexpr PortType typeToPortType<RgbaValue>()
	{
		return PortType::RGBAPixels;
	}

	template<>
	constexpr PortType typeToPortType<RealValue>()
	{
		return PortType::GrayscaleRealPixels;
	}

	template<>
	constexpr PortType typeToPortType<ComplexValue>()
	{
		return PortType::GrayscaleComplexPixels;
	}


	struct PortInfo
	{
		PortType type;
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