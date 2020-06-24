//@	{
//@	  "targets":[{"name":"plan.o", "type":"object", "pkgconfig_libs":["fftw3"]}]
//@	}

#include "./plan.hpp"
#include "utils/datablock.hpp"

Texpainter::Dft::Plan::Plan(Size2d size, Direction dir)
{
	auto const n = size.area();
	auto input_buff = std::make_unique<std::complex<double>>(n);
	auto output_buff = std::make_unique<std::complex<double>>(n);
	std::fill(input_buff.get(), input_buff.get() + n, 0);
	auto input_buff_ptr = reinterpret_cast<fftw_complex*>(input_buff.get());
	auto output_buff_ptr = reinterpret_cast<fftw_complex*>(output_buff.get());
	m_plan = std::unique_ptr<PlanType, PlanDeleter>{fftw_plan_dft_2d(size.height(),
	                                                                 size.width(),
	                                                                 input_buff_ptr,
	                                                                 output_buff_ptr,
	                                                                 static_cast<int>(dir),
	                                                                 FFTW_MEASURE),
	                                                PlanDeleter{}};
}