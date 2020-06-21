//@	{
//@	  "targets":[{"name":"fourier_transform.o", "type":"object", "pkgconfig_libs":["fftw3"]}]
//@	}

#include "./fourier_transform.hpp"

#include <cstring>

Texpainter::Model::BasicImage<std::complex<double>> Texpainter::Generators::FourierTransform::
operator()(Span2d<double const> vals_in)
{
	auto const w = vals_in.width();
	auto const h = vals_in.height();
	Model::BasicImage<std::complex<double>> ret{w, h};
	Model::BasicImage<std::complex<double>> input_buffer{w, h};
	auto input_buffer_ptr = reinterpret_cast<fftw_complex*>(std::data(input_buffer.pixels()));
	auto fftw_out_ptr = reinterpret_cast<fftw_complex*>(std::data(ret.pixels()));
	if(vals_in.size() != m_plan_fwd.m_size) [[unlikely]]
		{
			memset(static_cast<void*>(std::data(input_buffer.pixels())),
			       0,
			       input_buffer.area() * sizeof(std::complex<double>));
			auto plan = fftw_plan_dft_2d(h, w, input_buffer_ptr, fftw_out_ptr, FFTW_FORWARD, FFTW_MEASURE);
			m_plan_fwd.m_size = vals_in.size();
			fftw_destroy_plan(m_plan_fwd.m_plan);
			m_plan_fwd.m_plan = plan;
		}

	auto sign_row = 1;
	for(uint32_t row = 0; row < h; ++row)
	{
		auto sign_col = 1;
		for(uint32_t col = 0; col < w; ++col)
		{
			input_buffer(col, row) = std::complex<double>{vals_in(col, row) * sign_row * sign_col};
			sign_col *= -1;
		}
		sign_row *= -1;
	}

	fftw_execute_dft(m_plan_fwd.m_plan, input_buffer_ptr, fftw_out_ptr);
	std::ranges::for_each(ret.pixels(), [area = vals_in.area()](auto& val) { return val /= area; });

	return ret;
}

Texpainter::Model::BasicImage<double> Texpainter::Generators::FourierTransform::
operator()(Span2d<std::complex<double> const> vals_in)
{
	auto const w = vals_in.width();
	auto const h = vals_in.height();

	Model::BasicImage<std::complex<double>> output_buffer{w, h};
	auto fftw_out_ptr = reinterpret_cast<fftw_complex*>(std::data(output_buffer.pixels()));
	if(vals_in.size() != m_plan_bkwd.m_size) [[unlikely]]
		{
			Model::BasicImage<std::complex<double>> tmp{w, h};
			auto ptr = reinterpret_cast<fftw_complex*>(std::data(tmp.pixels()));
			memset(ptr, 0, tmp.area() * sizeof(std::complex<double>));
			auto plan = fftw_plan_dft_2d(h, w, ptr, fftw_out_ptr, FFTW_BACKWARD, FFTW_MEASURE);
			m_plan_bkwd.m_size = vals_in.size();
			fftw_destroy_plan(m_plan_bkwd.m_plan);
			m_plan_bkwd.m_plan = plan;
		}

	auto input_buffer_ptr =
	   reinterpret_cast<fftw_complex*>(const_cast<std::complex<double>*>(std::data(vals_in)));
	fftw_execute_dft(m_plan_bkwd.m_plan, input_buffer_ptr, fftw_out_ptr);
	Texpainter::Model::BasicImage<double> ret{w, h};
	auto sign_row = 1;
	for(uint32_t row = 0; row < h; ++row)
	{
		auto sign_col = 1;
		for(uint32_t col = 0; col < w; ++col)
		{
			ret(col, row) = output_buffer(col, row).real() * sign_col * sign_row;
			sign_col *= -1;
		}
		sign_row *= -1;
	}
	return ret;
}


Texpainter::Generators::FourierTransform::~FourierTransform()
{
	fftw_destroy_plan(m_plan_fwd.m_plan);
	fftw_destroy_plan(m_plan_bkwd.m_plan);
}
