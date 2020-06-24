//@	{
//@	  "targets":[{"name":"fourier_transform.hpp", "type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"fourier_transform.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_GENERATORS_FOURIERTRANSFORM_HPP
#define TEXPAINTER_GENERATORS_FOURIERTRANSFORM_HPP

#include "model/image.hpp"
#include "dft/plan.hpp"

namespace Texpainter::Generators
{
	class FourierTransform
	{
	public:
		Model::BasicImage<std::complex<double>> operator()(Span2d<double const> vals_in);

		Model::BasicImage<double> operator()(Span2d<std::complex<double> const> vals_in);

	private:
		template<Dft::Direction dir>
		struct PlanData
		{
			PlanData(): m_size{0, 0}
			{
			}
			Dft::Plan<dir> m_plan;
			Size2d m_size;
		};

		PlanData<Dft::Direction::Forward> m_plan_fwd;
		PlanData<Dft::Direction::Backward> m_plan_bkwd;
	};
}

#endif