//@	{
//@	 "targets":[{"name":"gaussian_blur.o", "type":"object"}]
//@	}

#include "./gaussian_blur.hpp"

#include "utils/frequency.hpp"
#include "utils/angle.hpp"

#include <cmath>

using Texpainter::SpatialFrequency;
using Texpainter::Angle;
using Texpainter::vec2_t;
using Texpainter::Str;
using Texpainter::Frequency;

namespace
{
	auto generateKernel(GaussianBlur::Size2d size, SpatialFrequency f_0, Angle/* θ*/)
	{
		GaussianBlur::BasicImage<GaussianBlur::RealValue> ret{size};

		auto O = 0.5*vec2_t{static_cast<double>(size.width()), static_cast<double>(size.height())};
		for(uint32_t row = 0; row < size.height(); ++row)
		{
			for(uint32_t col = 0; col < size.width(); ++col)
			{
				auto P = vec2_t{static_cast<double>(col), static_cast<double>(row)};
				auto f = SpatialFrequency{P - O}/f_0;
				ret(col, row) = std::exp2(-0.5*Texpainter::dot(f, f));
			}
		}

		return ret;
	}
}

void GaussianBlur::ImageProcessor::operator()(ImgProcArg<InterfaceDescriptor> const& args) const
{
	auto const size = args.size();

	if(!m_kernel.has_value() || m_kernel->size() != size) [[unlikely]]
	{
		auto const ξ_0 = Frequency{m_params.find<Str{"ξ_0"}>()->value()};
		auto const η_0 = Frequency{m_params.find<Str{"η_0"}>()->value()};
		auto const θ = Angle{0.5*m_params.find<Str{"θ"}>()->value(), Angle::Turns{}};

		m_kernel = generateKernel(size, SpatialFrequency{ξ_0, η_0}, Angle{θ});
	}

	auto kernel = m_kernel->pixels();

	for(uint32_t row = 0; row < size.height(); ++row)
	{
		for(uint32_t col = 0; col < size.width(); ++col)
		{
			args.output<0>(col, row) = kernel(col, row) * args.input<0>(col, row);
		}
	}
}