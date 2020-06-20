//@	{"targets":[{"name":"gaussian_freq_2d.hpp", "type":"include"}]}

#ifndef TEXPAINTER_GENERATORS_BUTTERWORTHFREQ2D_HPP
#define TEXPAINTER_GENERATORS_BUTTERWORTHFREQ2D_HPP

#include "./pointwise_transform.hpp"

#include "model/image.hpp"
#include "utils/angle.hpp"

#include <cmath>

namespace Texpainter::Generators
{
	class GaussianFreq2dKernel
	{
	public:
		explicit GaussianFreq2dKernel(Size2d size, Angle ϴ, SpatialFrequency ω_c):
		   m_x_0{size.width() / 2.0},
		   m_y_0{size.height() / 2.0},
		   m_rot_vec_ξ{cos(ϴ), -sin(ϴ)},
		   m_rot_vec_η{sin(ϴ), cos(ϴ)},
		   m_ω_c{ω_c}
		{
		}

		auto operator()(auto col, auto row, auto val) const
		{
			auto const ξ = static_cast<float>(col) - m_x_0;
			auto const η = static_cast<float>(row) - m_y_0;
			auto const ξ_ = ξ * cos(m_ϴ) - η * sin(m_ϴ);
			auto const η_ = ξ * sin(m_ϴ) + η * cos(m_ϴ);
			auto const H = exp(-(ξ_ * ξ_ * m_α_y + η_ * η_ * m_α_x + m_α_x * m_α_y));
			return val * H;
		}

		static constexpr auto aMin()
		{
			return 1.0 / 1024;
		}


	private:
		vec2_t m_O;
		vec2_t m_rot_vec_ξ;
		vec2_t m_rot_vec_η;
		SpatialFrequency m_ω_c;
	};

	class GaussianFreq2d
	{
	public:
		explicit GaussianFreq2d(Size2d size, Angle ϴ, SpatialFrequency ω_c):
		   m_f{GaussianFreq2dKernel{size, ϴ, ω_c}}
		{
		}

		Model::BasicImage<std::complex<double>> operator()(Span2d<std::complex<double> const> in)
		{
			return m_f(in);
		}

		static constexpr auto aMin()
		{
			return GaussianFreq2dKernel::aMin();
		}

	private:
		PointwiseTransform<GaussianFreq2dKernel> m_f;
	};
}

#endif