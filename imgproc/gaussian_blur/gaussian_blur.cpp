//@	{
//@	 "targets":[{"name":"gaussian_blur.o", "type":"object"}]
//@	}

#include "./gaussian_blur.hpp"

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
	auto generateKernel(GaussianBlur::Size2d size, SpatialFrequency f_0, Angle /* θ*/)
	{
		GaussianBlur::BasicImage<GaussianBlur::RealValue> ret{size};

		auto O =
		    0.5 * vec2_t{static_cast<double>(size.width()), static_cast<double>(size.height())};
		auto d = sqrt(size.area());
		f_0 *= vec2_t{d, d};

		for(uint32_t row = 0; row < size.height(); ++row)
		{
			for(uint32_t col = 0; col < size.width(); ++col)
			{
				auto P        = vec2_t{static_cast<double>(col), static_cast<double>(row)};
				auto f        = SpatialFrequency{P - O} / f_0;
				ret(col, row) = std::exp2(-0.5 * Texpainter::dot(f, f));
			}
		}

		return ret;
	}

	auto frequency(GaussianBlur::ParamValue val)
	{
		return Frequency{std::exp2(std::lerp(-16.0, 0.0, val.value()))};
	}
}

void GaussianBlur::ImageProcessor::operator()(ImgProcArg<InterfaceDescriptor> const& args) const
{
	auto const size = args.size();

	if(!m_kernel.has_value() || m_kernel->size() != size) [[unlikely]]
		{
			auto const ξ_0 = frequency(*m_params.find<Str{"ξ_0"}>());
			auto const η_0 = frequency(*m_params.find<Str{"η_0"}>());
			auto const θ   = Angle{0.5 * m_params.find<Str{"θ"}>()->value(), Angle::Turns{}};

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