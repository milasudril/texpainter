//@	{"targets":[{"name":"butterworth_freq_1d.hpp", "type":"include"}]}

#ifndef TEXPAINTER_GENERATORS_BUTTERWORTHFREQ1D_HPP
#define TEXPAINTER_GENERATORS_BUTTERWORTHFREQ1D_HPP

#include "./pointwise_transform.hpp"

#include "model/image.hpp"
#include "utils/angle.hpp"
#include "utils/frequency.hpp"

namespace Texpainter::Generators
{
	class ButterworthFreq1dKernel
	{
	public:
		explicit ButterworthFreq1dKernel(Size2d size, Angle ϴ, Frequency ω_c):
		   m_O{size.width() / 2.0, size.height() / 2.0},
		   m_ϴ{ϴ},
		   m_ω_c{ω_c}
		{
		}

		auto operator()(auto col, auto row, auto val) const
		{
			auto pos = vec2_t{static_cast<double>(col), static_cast<double>(row)} - m_O;
			auto const ξ = Frequency{pos[0]};
			auto const η = Frequency{pos[1]};

			auto const ω = ξ * cos(m_ϴ) - η * sin(m_ϴ);
			auto const Ϙω = ω/m_ω_c;
			auto const H = 1.0 / sqrt(Ϙω*Ϙω + 1.0);
			return val * H;
		}

	private:
		vec2_t m_O;
		Angle m_ϴ;
		Frequency m_ω_c;
	};

	class ButterworthFreq1d
	{
	public:
		explicit ButterworthFreq1d(Size2d size, Angle ϴ, Frequency ω_c):
		   m_f{ButterworthFreq1dKernel{size, ϴ, ω_c}}
		{
		}

		Model::BasicImage<std::complex<double>> operator()(Span2d<std::complex<double> const> in)
		{
			return m_f(in);
		}

	private:
		PointwiseTransform<ButterworthFreq1dKernel> m_f;
	};
}

#endif