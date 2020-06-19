//@	{"targets":[{"name":"butterworth_freq_1d.hpp", "type":"include"}]}

#ifndef TEXPAINTER_GENERATORS_BUTTERWORTHFREQ1D_HPP
#define TEXPAINTER_GENERATORS_BUTTERWORTHFREQ1D_HPP

#include "./pointwise_transform.hpp"

#include "model/image.hpp"
#include "utils/angle.hpp"

namespace Texpainter::Generators
{
	class ButterworthFreq1dKernel
	{
	public:
		explicit ButterworthFreq1dKernel(Size2d size, Angle ϴ, double a):
		   m_x_0{size.width() / 2.0},
		   m_y_0{size.height() / 2.0},
		   m_ϴ{ϴ},
		   m_α{a * a}
		{
		}

		auto operator()(auto col, auto row, auto val) const
		{
			auto const ξ = static_cast<float>(col) - m_x_0;
			auto const η = static_cast<float>(row) - m_y_0;

			auto const ω = ξ * cos(m_ϴ) - η * sin(m_ϴ);

			auto const denom = std::max(sqrt(ω * ω + m_α), aMin() * aMin());
			auto const H = 1.0f / denom;
			return val * H;
		}

		static constexpr auto aMin()
		{
			return 1.0 / 1024;
		}

		static constexpr auto ϴMin()
		{
			return Angle{0.0, Angle::Turns{}};
		}

		static constexpr auto ϴMax()
		{
			return Angle{0.5, Angle::Turns{}};
		}


	private:
		double m_x_0;
		double m_y_0;
		Angle m_ϴ;
		double m_α;
	};

	class ButterworthFreq1d
	{
	public:
		explicit ButterworthFreq1d(Size2d size, Angle ϴ, double a):
		   m_f{ButterworthFreq1dKernel{size, ϴ, a}}
		{
		}

		Model::BasicImage<std::complex<double>> operator()(Span2d<std::complex<double> const> in)
		{
			return m_f(in);
		}

		static constexpr auto aMin()
		{
			return ButterworthFreq1dKernel::aMin();
		}

		static constexpr auto ϴMin()
		{
			return 0.0f;
		}

		static constexpr auto ϴMax()
		{
			return std::numbers::pi;
		}

	private:
		PointwiseTransform<ButterworthFreq1dKernel> m_f;
	};
}

#endif