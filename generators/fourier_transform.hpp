//@	{
//@	  "targets":[{"name":"fourier_transform.hpp", "type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"fourier_transform.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_GENERATORS_FOURIERTRANSFORM_HPP
#define TEXPAINTER_GENERATORS_FOURIERTRANSFORM_HPP

#include "model/image.hpp"

#include <fftw3.h>
#include <complex>

namespace Texpainter::Generators
{
	class FourierTransform
	{
	public:
		FourierTransform(): m_plan{nullptr}, m_w_old{0}, m_h_old{0}
		{
		}

		Model::BasicImage<std::complex<float>> operator()(Span2d<float const> vals_in);

		~FourierTransform();

	private:
		fftwf_plan m_plan;
		uint32_t m_w_old;
		uint32_t m_h_old;
	};
}

#endif