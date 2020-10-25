//@	{
//@	 "targets":[{"name":"port_type.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_PORTTYPE_HPP
#define TEXPAINTER_FILTERGRAPH_PORTTYPE_HPP

#include "pixel_store/pixel.hpp"
#include "utils/size_2d.hpp"

#include "libenum/enum.hpp"

#include <complex>
#include <cstddef>
#include <memory>

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
		using type = std::unique_ptr<RgbaValue[]>;

		static type createValue(Size2d size) { return std::make_unique<RgbaValue[]>(size.area()); }
	};

	template<>
	struct PortTypeToType<PortType::GrayscaleRealPixels>
	{
		using type = std::unique_ptr<RealValue[]>;

		static type createValue(Size2d size) { return std::make_unique<RealValue[]>(size.area()); }
	};

	template<>
	struct PortTypeToType<PortType::GrayscaleComplexPixels>
	{
		using type = std::unique_ptr<ComplexValue[]>;

		static type createValue(Size2d size)
		{
			return std::make_unique<ComplexValue[]>(size.area());
		}
	};

#if 0
	template<class T>
	constexpr PortType typeToPortType() = delete;

	template<>
	constexpr PortType typeToPortType<std::unique_ptr<RgbaValue[]>>()
	{
		return PortType::RGBAPixels;
	}

	template<>
	constexpr PortType typeToPortType<std::unique_ptr<RealValue[]>>()
	{
		return PortType::GrayscaleRealPixels;
	}

	template<>
	constexpr PortType typeToPortType<std::unique_ptr<ComplexValue[]>>()
	{
		return PortType::GrayscaleComplexPixels;
	}
#endif
}
#endif