//@	{
//@	 "targets":[{"name":"port_type.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_PORTTYPE_HPP
#define TEXPAINTER_FILTERGRAPH_PORTTYPE_HPP

#include "pixel_store/pixel.hpp"

#include "libenum/variant.hpp"

#include <complex>
#include <cstddef>

namespace Texpainter::FilterGraph
{
	enum class PortType : size_t
	{
		RGBAPixels,
		GrayscaleRealPixels,
		GrayscaleComplexPixels
	};

	constexpr PortType begin(Enum::Empty<PortType>) { return PortType::RGBAPixels; }
	constexpr PortType end(Enum::Empty<PortType>)
	{
		return Enum::add(PortType::GrayscaleComplexPixels);
	}

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
}
#endif