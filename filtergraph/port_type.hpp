//@	{
//@	 "targets":[{"name":"port_type.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_PORTTYPE_HPP
#define TEXPAINTER_FILTERGRAPH_PORTTYPE_HPP

#include "pixel_store/rgba_value.hpp"
#include "pixel_store/palette.hpp"
#include "utils/size_2d.hpp"
#include "utils/angle.hpp"
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
		PointCloud,
		Palette,
		LineSegTree,
		VectorField,
		PolylineSet
	};

	constexpr PortType begin(Enum::Empty<PortType>) { return PortType::RgbaPixels; }
	constexpr PortType end(Enum::Empty<PortType>) { return Enum::add(PortType::PolylineSet); }

	using RgbaValue      = PixelStore::RgbaValue;
	using RealValue      = float;
	using ComplexValue   = std::complex<RealValue>;
	using Palette        = PixelStore::RgbaPalette<16>;
	using TopographyInfo = PixelStore::TopographyInfo;
	using PolylineSet    = std::vector<std::vector<vec2_t>>;
	using Vector         = vec2_t;

	struct LineSegTree
	{
		std::vector<std::pair<vec2_t, LineSegTree>> data;
	};

	struct ImageCoordinates
	{
		uint32_t x;
		uint32_t y;
	};

	struct SpawnSpot
	{
		ImageCoordinates loc;
		Angle rot;
		float scale;
	};

	template<PortType id>
	struct PortTypeToType;

	template<>
	struct PortTypeToType<PortType::RgbaPixels>
	{
		using type = std::unique_ptr<RgbaValue[]>;

		static type createValue(Size2d size) { return std::make_unique<RgbaValue[]>(area(size)); }
	};

	template<>
	struct PortTypeToType<PortType::VectorField>
	{
		using type = std::unique_ptr<Vector[]>;

		static type createValue(Size2d size) { return std::make_unique<Vector[]>(area(size)); }
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
	struct PortTypeToType<PortType::PointCloud>
	{
		using type = std::vector<SpawnSpot>;

		static type createValue(Size2d) { return type{}; }
	};

	template<>
	struct PortTypeToType<PortType::Palette>
	{
		using type = Palette;

		static type createValue(Size2d) { return type{}; }
	};

	template<>
	struct PortTypeToType<PortType::PolylineSet>
	{
		using type = PolylineSet;

		static type createValue(Size2d) { return type{}; }
	};

	template<>
	struct PortTypeToType<PortType::LineSegTree>
	{
		using type = std::vector<LineSegTree>;
		static type createValue(Size2d) { return type{}; }
	};
}
#endif