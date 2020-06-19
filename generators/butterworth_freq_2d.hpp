//@	{"targets":[{"name":"butterworth_freq_2d.hpp", "type":"include"}]}

#ifndef TEXPAINTER_GENERATORS_BUTTERWORTHFREQ2D_HPP
#define TEXPAINTER_GENERATORS_BUTTERWORTHFREQ2D_HPP

#include "./pointwise_transform.hpp"

#include "model/image.hpp"


namespace Texpainter::Generators
{
	class ButterworthFreq2dKernel
	{
	public:
		explicit ButterworthFreq2dKernel(std::tuple<uint32_t, uint32_t> size, double a_x, double a_y):
		   m_x_0{std::get<0>(size) / 2.0},
		   m_y_0{std::get<1>(size) / 2.0},
		   m_α_x{a_x * a_x},
		   m_α_y{a_y * a_y}
		{
		}

		auto operator()(auto col, auto row, auto val) const
		{
			auto const ξ = std::abs(static_cast<float>(col) - m_x_0);
			auto const η = std::abs(static_cast<float>(row) - m_y_0);
			auto const denom = std::max(ξ * ξ * m_α_y + η * η * m_α_x + m_α_x * m_α_y, aMin() * aMin());
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
		double m_α_x;
		double m_α_y;
	};

	class ButterworthFreq2d
	{
	public:
		explicit ButterworthFreq2d(std::tuple<uint32_t, uint32_t> size, double a_x, double a_y):
		   m_f{ButterworthFreq2dKernel{size, a_x, a_y}}
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