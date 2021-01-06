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

	constexpr double sizeScaleFactor(double val) { return fromExponent(val * MinumSize); }

	constexpr double sizeFromArea(Size2d size, double val)
	{
		return std::max(2.0, std::sqrt(area(size)) * sizeScaleFactor(val));
	}

	constexpr double sizeFromWidth(Size2d size, double val)
	{
		return std::max(2.0, size.width() * sizeScaleFactor(val));
	}

	constexpr double sizeFromHeight(Size2d size, double val)
	{
		return std::max(2.0, size.height() * sizeScaleFactor(val));
	}

}

#endif