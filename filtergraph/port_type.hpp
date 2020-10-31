//@	{
//@	 "targets":[{"name":"port_type.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_PORTTYPE_HPP
#define TEXPAINTER_FILTERGRAPH_PORTTYPE_HPP

#include "pixel_store/pixel.hpp"
#include "pixel_store/palette.hpp"
#include "utils/size_2d.hpp"

#include "libenum/enum.hpp"

#include <complex>
#include <cstddef>
#include <memory>

namespace Texpainter::FilterGraph
{
	enum class PortType : size_t
	{
		RgbaPixels,
		GrayscaleRealPixels,
		GrayscaleComplexPixels,
		RgbaValue,
		RealValue,
		ComplexValue,
		Palette
	};

	constexpr PortType begin(Enum::Empty<PortType>) { return PortType::RgbaPixels; }
	constexpr PortType end(Enum::Empty<PortType>) { return Enum::add(PortType::Palette); }

	using RgbaValue    = Texpainter::PixelStore::Pixel;
	using RealValue    = double;
	using ComplexValue = std::complex<RealValue>;
	using Palette      = PixelStore::Palette<16>;

	template<PortType id>
	struct PortTypeToType;

	template<>
	struct PortTypeToType<PortType::RgbaPixels>
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

	template<>
	struct PortTypeToType<PortType::RgbaValue>
	{
		using type = RgbaValue;

		static type createValue(Size2d) { return RgbaValue{}; }
	};

	template<>
	struct PortTypeToType<PortType::RealValue>
	{
		using type = RealValue;

		static type createValue(Size2d) { return RealValue{}; }
	};

	template<>
	struct PortTypeToType<PortType::ComplexValue>
	{
		using type = ComplexValue;

		static type createValue(Size2d) { return ComplexValue{}; }
	};

	template<>
	struct PortTypeToType<PortType::Palette>
	{
		using type = Palette;

		static type createValue(Size2d) { return Palette{}; }
	};
}
#endif