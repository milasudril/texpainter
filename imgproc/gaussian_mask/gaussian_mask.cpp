//@	{
//@	 "targets":[{"name":"gaussian_mask.o", "type":"object"}]
//@	}

#include "./gaussian_mask.hpp"

#include "utils/frequency.hpp"
#include "utils/angle.hpp"

#include <cmath>

using Texpainter::Angle;
using Texpainter::Frequency;
using Texpainter::SpatialFrequency;
using Texpainter::Str;
using Texpainter::vec2_t;

namespace
{
	auto frequency(GaussianMask::ParamValue val)
	{
		return Frequency{std::exp2(std::lerp(-16.0, 0.0, val.value()))};
	}
}

void GaussianMask::ImageProcessor::operator()(ImgProcArg<InterfaceDescriptor> const& args) const
{
	auto const size = args.size();

	auto const ξ_0 = frequency(*m_params.find<Str{"ξ_0"}>());
	auto const η_0 = frequency(*m_params.find<Str{"η_0"}>());
	auto const θ   = Angle{0.5 * m_params.find<Str{"θ"}>()->value(), Angle::Turns{}};

	auto const rot_vec_ξ = vec2_t{cos(θ), -sin(θ)};
	auto const rot_vec_η = vec2_t{sin(θ), cos(θ)};

	auto const O =
	    0.5 * vec2_t{static_cast<double>(size.width()), static_cast<double>(size.height())};
	auto const d   = sqrt(size.area());
	auto const f_0 = SpatialFrequency{ξ_0, η_0} * vec2_t{d, d};

	for(uint32_t row = 0; row < size.height(); ++row)
	{
		for(uint32_t col = 0; col < size.width(); ++col)
		{
			auto P = vec2_t{static_cast<double>(col), static_cast<double>(row)};
			auto f = transform(SpatialFrequency{P - O}, rot_vec_ξ, rot_vec_η) / f_0;
			args.output<0>(col, row) = std::exp2(-0.5 * Texpainter::dot(f, f));
		}
	}
}