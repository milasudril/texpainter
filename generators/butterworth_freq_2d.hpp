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
		explicit ButterworthFreq2dKernel(std::tuple<uint32_t, uint32_t> size, float a_x, float a_y):
		   m_x_0{std::get<0>(size) / 2.0f},
		   m_y_0{std::get<1>(size) / 2.0f},
		   m_a_x{a_x},
		   m_a_y{a_y}
		{
		}

		auto operator()(auto col, auto row, auto val) const
		{
			auto const xi = std::abs(static_cast<float>(col) - m_x_0);
			auto const eta = std::abs(static_cast<float>(row) - m_y_0);
			auto const r2 = std::max(xi * xi + eta * eta, 1.0f / (1024.0f));
			auto H = 1.0f / (r2 + m_a_x * m_a_x);
			return val * H;
		}

	private:
		float m_x_0;
		float m_y_0;
		float m_a_x;
		float m_a_y;
	};

	class ButterworthFreq2d
	{
	public:
		explicit ButterworthFreq2d(std::tuple<uint32_t, uint32_t> size, float a_x, float a_y):
		   m_f{ButterworthFreq2dKernel{size, a_x, a_y}}
		{
		}

		Model::BasicImage<std::complex<float>> operator()(Span2d<std::complex<float> const> in)
		{
			return m_f(in);
		}

	private:
		PointwiseTransform<ButterworthFreq2dKernel> m_f;
	};
}

#endif