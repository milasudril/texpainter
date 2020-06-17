//@	{
//@	  "targets":[{"name":"fourier_transform.o", "type":"object", "pkgconfig_libs":["fftw3f"]}]
//@	}

#include "./fourier_transform.hpp"

Texpainter::Model::BasicImage<std::complex<float>> Texpainter::Generators::FourierTransform::
operator()(Span2d<float const> vals_in)
{
	auto const w = vals_in.width();
	auto const h = vals_in.height();
	Texpainter::Model::BasicImage<std::complex<float>> ret{w / 2 + 1, h};
	auto fftw_out_ptr = reinterpret_cast<fftwf_complex*>(std::data(ret.pixels()));
	if(w != m_w_old || h != m_h_old)
	{
		auto tmp = Texpainter::Model::BasicImage<float>{w, h};
		memset(std::data(tmp.pixels()), 0, tmp.pixels().area() * sizeof(float));
		auto plan = fftwf_plan_dft_r2c_2d(h, w, std::data(tmp.pixels()), fftw_out_ptr, FFTW_MEASURE);
		m_w_old = w;
		m_h_old = h;
		fftwf_destroy_plan(m_plan);
		m_plan = plan;
	}
	fftwf_execute_dft_r2c(m_plan, const_cast<float*>(std::data(vals_in)), fftw_out_ptr);
	return ret;
}

Texpainter::Generators::FourierTransform::~FourierTransform()
{
	fftwf_destroy_plan(m_plan);
}