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
		Model::BasicImage<Dft::Plan::sample_type> operator()(Span2d<double const> vals_in);

		Model::BasicImage<double> operator()(Span2d<Dft::Plan::sample_type const> vals_in);

	private:
		struct PlanData
		{
			PlanData(): m_size{0, 0}
			{
			}
			Dft::Plan m_plan;
			Size2d m_size;
		};

		PlanData m_plan_fwd;
		PlanData m_plan_bkwd;
	};
}

#endif