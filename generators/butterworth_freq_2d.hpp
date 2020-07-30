//@	{"targets":[{"name":"butterworth_freq_2d.hpp", "type":"include"}]}

#ifndef TEXPAINTER_GENERATORS_BUTTERWORTHFREQ2D_HPP
#define TEXPAINTER_GENERATORS_BUTTERWORTHFREQ2D_HPP

#include "./pointwise_transform.hpp"

#include "pixel_store/image.hpp"
#include "utils/angle.hpp"
#include "utils/frequency.hpp"

namespace Texpainter::Generators
{
	class ButterworthFreq2dKernel
	{
	public:
		explicit ButterworthFreq2dKernel(Size2d size, Angle ϴ, SpatialFrequency ω_c)
		    : m_O{size.width() / 2.0, size.height() / 2.0}
		    , m_rot_vec_ξ{cos(ϴ), -sin(ϴ)}
		    , m_rot_vec_η{sin(ϴ), cos(ϴ)}
		    , m_ω_c{ω_c}
		{
		}

		auto operator()(auto col, auto row, auto val) const
		{
			auto ω =
			    SpatialFrequency{vec2_t{static_cast<double>(col), static_cast<double>(row)} - m_O};
			ω             = transform(ω, m_rot_vec_ξ, m_rot_vec_η);
			auto const Ϙω = ω / m_ω_c;
			auto const H  = 1.0 / (dot(Ϙω, Ϙω) + 1.0);
			return val * H;
		}

	private:
		vec2_t m_O;
		vec2_t m_rot_vec_ξ;
		vec2_t m_rot_vec_η;
		SpatialFrequency m_ω_c;
	};

	class ButterworthFreq2d
	{
	public:
		explicit ButterworthFreq2d(Size2d size, Angle ϴ, SpatialFrequency ω_c)
		    : m_f{ButterworthFreq2dKernel{size, ϴ, ω_c}}
		{
		}

		PixelStore::BasicImage<std::complex<double>> operator()(
		    Span2d<std::complex<double> const> in)
		{
			return m_f(in);
		}

	private:
		PointwiseTransform<ButterworthFreq2dKernel> m_f;
	};
}

#endif