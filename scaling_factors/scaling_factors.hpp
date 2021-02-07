//@	{
//@	 "targets":[{"name":"port_type.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_SCALINGFACTORS_SCALINGFACTORS_HPP
#define TEXPAINTER_SCALINGFACTORS_SCALINGFACTORS_HPP

#include "utils/exponent.hpp"
#include "utils/size_2d.hpp"

#include <algorithm>

namespace Texpainter::ScalingFactors
{
	constexpr auto MinumSize = Exponent{-15.0};

	constexpr double sizeScaleFactor(double val) { return fromExponent((1.0 - val) * MinumSize); }

	constexpr double areaScaleFactor(double val) { return fromExponent(2.0*(1.0 - val) * MinumSize); }

	constexpr double sizeFromGeomMean(Size2d size, double val)
	{
		return std::max(2.0, std::sqrt(area(size)) * sizeScaleFactor(val));
	}

	constexpr double sizeFromMin(Size2d size, double val)
	{
		return std::max(2.0, std::min(size.width(), size.height()) * sizeScaleFactor(val));
	}

	constexpr double sizeFromMax(Size2d size, double val)
	{
		return std::max(2.0, std::max(size.width(), size.height()) * sizeScaleFactor(val));
	}

	constexpr double sizeFromWidth(Size2d size, double val)
	{
		return std::max(2.0, size.width() * sizeScaleFactor(val));
	}

	constexpr double sizeFromHeight(Size2d size, double val)
	{
		return std::max(2.0, size.height() * sizeScaleFactor(val));
	}

	constexpr double sizeFromArea(Size2d size, double val)
	{
		return std::max(4.0, area(size) * areaScaleFactor(val));
	}

}

#endif