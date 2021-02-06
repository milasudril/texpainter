//@	{
//@	 "targets":[{"name":"port_type.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_PORTTYPE_HPP
#define TEXPAINTER_FILTERGRAPH_PORTTYPE_HPP

#include "pixel_store/pixel.hpp"
#include "pixel_store/palette.hpp"
#include "utils/size_2d.hpp"
#include "pixel_store/topography_info.hpp"

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
		TopographyData,
		Palette
	};

	constexpr PortType begin(Enum::Empty<PortType>) { return PortType::RgbaPixels; }
	constexpr PortType end(Enum::Empty<PortType>) { return Enum::add(PortType::Palette); }

	using RgbaValue      = PixelStore::Pixel;
	using RealValue      = double;
	using ComplexValue   = std::complex<RealValue>;
	using Palette        = PixelStore::Palette<16>;
	using TopographyInfo = Model::TopographyInfo;

	template<PortType id>
	struct PortTypeToType;

	template<>
	struct PortTypeToType<PortType::RgbaPixels>
	{
		using type = std::unique_ptr<RgbaValue[]>;

		static type createValue(Size2d size) { return std::make_unique<RgbaValue[]>(area(size)); }
	};

	template<>
	struct PortTypeToType<PortType::GrayscaleRealPixels>
	{
		using type = std::unique_ptr<RealValue[]>;

		static type createValue(Size2d size) { return std::make_unique<RealValue[]>(area(size)); }
	};

	template<>
	struct PortTypeToType<PortType::GrayscaleComplexPixels>
	{
		using type = std::unique_ptr<ComplexValue[]>;

		static type createValue(Size2d size)
		{
			return std::make_unique<ComplexValue[]>(area(size));
		}
	};

	template<>
	struct PortTypeToType<PortType::TopographyData>
	{
		using type = std::unique_ptr<TopographyInfo[]>;

		static type createValue(Size2d size)
		{
			return std::make_unique<TopographyInfo[]>(area(size));
		}
	};

	template<>
	struct PortTypeToType<PortType::Palette>
	{
		using type = Palette;

		static type createValue(Size2d) { return Palette{}; }
	};
}
#endif