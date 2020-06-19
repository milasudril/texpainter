//@	{"targets":[{"name":"butterworth_freq_2d.hpp", "type":"include"}]}

#ifndef TEXPAINTER_GENERATORS_BUTTERWORTHFREQ2D_HPP
#define TEXPAINTER_GENERATORS_BUTTERWORTHFREQ2D_HPP

#include "./pointwise_transform.hpp"

#include "model/image.hpp"
#include "utils/angle.hpp"

namespace Texpainter::Generators
{
	class ButterworthFreq2dKernel
	{
	public:
		explicit ButterworthFreq2dKernel(Size2d size, Angle ϴ, double a_x, double a_y):
		   m_x_0{size.width() / 2.0},
		   m_y_0{size.height() / 2.0},
		   m_ϴ{ϴ},
		   m_α_x{a_x * a_x},
		   m_α_y{a_y * a_y}
		{
		}

		auto operator()(auto col, auto row, auto val) const
		{
			auto const ξ = static_cast<float>(col) - m_x_0;
			auto const η = static_cast<float>(row) - m_y_0;
			auto const ξ_ = ξ * cos(m_ϴ) - η * sin(m_ϴ);
			auto const η_ = ξ * sin(m_ϴ) + η * cos(m_ϴ);
			auto const denom = std::max(ξ_ * ξ_ * m_α_y + η_ * η_ * m_α_x + m_α_x * m_α_y, aMin() * aMin());
			auto const H = 1.0 / denom;
			return val * H;
		}

		static constexpr auto aMin()
		{
			return 1.0 / 1024;
		}


	private:
		double m_x_0;
		double m_y_0;
		Angle m_ϴ;
		double m_α_x;
		double m_α_y;
	};

	class ButterworthFreq2d
	{
	public:
		explicit ButterworthFreq2d(Size2d size, Angle ϴ, double a_x, double a_y):
		   m_f{ButterworthFreq2dKernel{size, ϴ, a_x, a_y}}
		{
		}

		Model::BasicImage<std::complex<double>> operator()(Span2d<std::complex<double> const> in)
		{
			return m_f(in);
		}

		static constexpr auto aMin()
		{
			return ButterworthFreq2dKernel::aMin();
		}

	private:
		PointwiseTransform<ButterworthFreq2dKernel> m_f;
	};
}

#endif