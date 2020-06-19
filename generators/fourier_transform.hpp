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
		Model::BasicImage<std::complex<double>> operator()(Span2d<float const> vals_in);

		Model::BasicImage<float> operator()(Span2d<std::complex<double> const> vals_in);

		~FourierTransform();

	private:
		struct PlanData
		{
			PlanData(): m_plan{nullptr}, m_size{0, 0}
			{
			}
			fftw_plan m_plan;
			Size2d m_size;
		};

		PlanData m_plan_fwd;
		PlanData m_plan_bkwd;
	};
}

#endif