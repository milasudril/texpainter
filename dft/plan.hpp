//@	{
//@	  "targets":[{"name":"plan.hpp", "type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"plan.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_DFT_PLAN_HPP
#define TEXPAINTER_DFT_PLAN_HPP

#include "utils/size_2d.hpp"

#include <fftw3.h>
#include <complex>
#include <memory>
#include <type_traits>
#include <cassert>

namespace Texpainter::Dft
{
	struct PlanDeleter
	{
		void operator()(fftw_plan plan)
		{
			if(plan != nullptr) { fftw_destroy_plan(plan); }
		}
	};

	enum class Direction : int
	{
		Forward = FFTW_FORWARD,
		Backward = FFTW_BACKWARD
	};

	class BasicPlan
	{
	public:
		explicit BasicPlan(Size2d size, Direction dir);

		BasicPlan(): m_plan{nullptr}
		{
		}

		using sample_type = std::complex<double>;

		void execute(sample_type const* input_buffer, sample_type* output_buffer) const
		{
			assert(valid());
			auto input_buffer_ptr = reinterpret_cast<fftw_complex*>(const_cast<sample_type*>(input_buffer));
			auto output_buffer_ptr = reinterpret_cast<fftw_complex*>(output_buffer);
			fftw_execute_dft(m_plan.get(), input_buffer_ptr, output_buffer_ptr);
		}

		bool valid() const
		{
			return m_plan != nullptr;
		}

	private:
		using PlanType = std::remove_pointer_t<fftw_plan>;
		std::unique_ptr<PlanType, PlanDeleter> m_plan;
	};

	template<Direction dir>
	class Plan: public BasicPlan
	{
	public:
		Plan(): BasicPlan{}
		{
		}

		explicit Plan(Size2d size): BasicPlan{size, dir}
		{
		}
	};
}

#endif