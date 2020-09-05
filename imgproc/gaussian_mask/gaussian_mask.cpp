//@	{
//@	 "targets":[{"name":"gaussian_mask.o", "type":"object"}]
//@	}

#include "./gaussian_mask.hpp"

#include "utils/angle.hpp"

#include <cmath>

using Texpainter::Angle;
using Texpainter::Str;
using Texpainter::vec2_t;

namespace
{
	auto sizeFromParam(GaussianMask::ParamValue val)
	{
		return 0.5*std::exp2(std::lerp(-16.0, 0.0, val.value()));
	}
}

void GaussianMask::ImageProcessor::operator()(ImgProcArg<InterfaceDescriptor> const& args) const
{
	auto const size = args.size();

	auto const r_x  = sizeFromParam(*m_params.find<Str{"Semi-axis 1"}>());
	auto const r_y = sizeFromParam(*m_params.find<Str{"Semi-axis 2"}>());
	auto const θ      = Angle{0.5 * m_params.find<Str{"Orientation"}>()->value(), Angle::Turns{}};

	auto const rot_vec_x = vec2_t{cos(θ), -sin(θ)};
	auto const rot_vec_y = vec2_t{sin(θ), cos(θ)};

	auto const O =
	    0.5 * vec2_t{static_cast<double>(size.width()), static_cast<double>(size.height())};
	auto const r_0 = sqrt(size.area())*vec2_t{r_x, r_y};

	for(uint32_t row = 0; row < size.height(); ++row)
	{
		for(uint32_t col = 0; col < size.width(); ++col)
		{
			auto P = vec2_t{static_cast<double>(col), static_cast<double>(row)};
			auto r = Texpainter::transform(P - O, rot_vec_x, rot_vec_y) / r_0;
			args.output<0>(col, row) = std::exp2(-0.5 * Texpainter::dot(r, r));
		}
	}
}