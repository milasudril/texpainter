//@	{
//@	  "targets":[{"name":"fourier_transform.o", "type":"object", "pkgconfig_libs":["fftw3f"]}]
//@	}

#include "./fourier_transform.hpp"

Texpainter::Model::BasicImage<std::complex<float>> Texpainter::Generators::FourierTransform::
operator()(Span2d<float const> vals_in)
{
	auto const w = vals_in.width();
	auto const h = vals_in.height();
	Model::BasicImage<std::complex<float>> ret{w, h};
	Model::BasicImage<std::complex<float>> input_buffer{w, h};
	auto input_buffer_ptr = reinterpret_cast<fftwf_complex*>(std::data(input_buffer.pixels()));
	auto fftw_out_ptr = reinterpret_cast<fftwf_complex*>(std::data(ret.pixels()));
	if(w != m_plan_fwd.m_w_old || h != m_plan_fwd.m_h_old) [[unlikely]]
		{
			memset(static_cast<void*>(std::data(input_buffer.pixels())),
			       0,
			       input_buffer.area() * sizeof(std::complex<float>));
			auto plan = fftwf_plan_dft_2d(h, w, input_buffer_ptr, fftw_out_ptr, FFTW_FORWARD, FFTW_MEASURE);
			m_plan_fwd.m_w_old = w;
			m_plan_fwd.m_h_old = h;
			fftwf_destroy_plan(m_plan_fwd.m_plan);
			m_plan_fwd.m_plan = plan;
		}

	auto sign_row = 1;
	for(uint32_t row = 0; row < h; ++row)
	{
		auto sign_col = 1;
		for(uint32_t col = 0; col < w; ++col)
		{
			input_buffer(col, row) = std::complex<float>{vals_in(col, row) * sign_row * sign_col};
			sign_col *= -1;
		}
		sign_row *= -1;
	}

	fftwf_execute_dft(m_plan_fwd.m_plan, input_buffer_ptr, fftw_out_ptr);
	std::ranges::for_each(ret.pixels(), [area = vals_in.area()](auto& val) { return val /= area; });

	return ret;
}

Texpainter::Model::BasicImage<float> Texpainter::Generators::FourierTransform::
operator()(Span2d<std::complex<float> const> vals_in)
{
	auto const w = vals_in.width();
	auto const h = vals_in.height();

	Model::BasicImage<std::complex<float>> output_buffer{w, h};
	auto fftw_out_ptr = reinterpret_cast<fftwf_complex*>(std::data(output_buffer.pixels()));
	if(w != m_plan_bkwd.m_w_old || h != m_plan_bkwd.m_h_old) [[unlikely]]
		{
			Model::BasicImage<std::complex<float>> tmp{w, h};
			auto ptr = reinterpret_cast<fftwf_complex*>(std::data(tmp.pixels()));
			memset(ptr, 0, tmp.area() * sizeof(std::complex<float>));
			auto plan = fftwf_plan_dft_2d(h, w, ptr, fftw_out_ptr, FFTW_BACKWARD, FFTW_MEASURE);
			m_plan_bkwd.m_w_old = w;
			m_plan_bkwd.m_h_old = h;
			fftwf_destroy_plan(m_plan_bkwd.m_plan);
			m_plan_bkwd.m_plan = plan;
		}

	auto input_buffer_ptr =
	   reinterpret_cast<fftwf_complex*>(const_cast<std::complex<float>*>(std::data(vals_in)));
	fftwf_execute_dft(m_plan_bkwd.m_plan, input_buffer_ptr, fftw_out_ptr);
	Texpainter::Model::BasicImage<float> ret{w, h};
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
	fftwf_destroy_plan(m_plan_fwd.m_plan);
	fftwf_destroy_plan(m_plan_bkwd.m_plan);
}