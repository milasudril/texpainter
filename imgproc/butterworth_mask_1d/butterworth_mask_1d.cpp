//@	{
//@	 "targets":[{"name":"butterworth_mask_1d.o", "type":"object"}]
//@	}

#include "./butterworth_mask_1d.hpp"

#include "utils/angle.hpp"

#include <cmath>
#include <cassert>

using Texpainter::Angle;
using Texpainter::Str;
using Texpainter::vec2_t;

namespace
{
	inline auto sizeFromParam(size_t size, ButterworthMask1d::ParamValue val)
	{
		return 0.5 * std::exp2(std::lerp(-std::log2(size), 0.0, val.value()));
	}

	auto orderFromParam(ButterworthMask1d::ParamValue val)
	{
		return static_cast<int>(std::nextafter(4.0, 0.0) * val.value()) + 1;
	}
}

void ButterworthMask1d::ImageProcessor::operator()(
    ImgProcArg<InterfaceDescriptor> const& args) const
{
	auto const size = args.size();

	auto const r =
	    sizeFromParam(static_cast<size_t>(sqrt(size.area())), *m_params.find<Str{"Radius"}>());
	auto const n = orderFromParam(*m_params.find<Str{"Order"}>());
	auto const θ = Angle{0.5 * m_params.find<Str{"Orientation"}>()->value(), Angle::Turns{}};

	auto const rot_vec_x = vec2_t{cos(θ), -sin(θ)};

	auto const O =
	    0.5 * vec2_t{static_cast<double>(size.width()), static_cast<double>(size.height())};
	auto const r_0 = sqrt(size.area()) * r;

	for(uint32_t row = 0; row < size.height(); ++row)
	{
		for(uint32_t col = 0; col < size.width(); ++col)
		{
			auto P = vec2_t{static_cast<double>(col), static_cast<double>(row)};
			auto r = (P - O) / r_0;
			args.output<0>(col, row) =
			    1.0 / sqrt(1 + std::pow(Texpainter::dot(r, rot_vec_x), 2 * n));
		}
	}
}