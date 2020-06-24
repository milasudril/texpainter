//@	{
//@	  "targets":[{"name":"fourier_transform.o", "type":"object", "pkgconfig_libs":["fftw3"]}]
//@	}

#include "./fourier_transform.hpp"

#include <cstring>

Texpainter::Model::BasicImage<Texpainter::Dft::Plan::sample_type> Texpainter::Generators::FourierTransform::
operator()(Span2d<double const> vals_in)
{
	if(vals_in.size() != m_plan_fwd.m_size) [[unlikely]]
		{
			m_plan_fwd.m_plan = Dft::Plan{vals_in.size(), Dft::Direction::Forward};
			m_plan_fwd.m_size = vals_in.size();
		}

	auto sign_row = 1;
	Model::BasicImage<std::complex<double>> input_buffer{vals_in.size()};
	for(uint32_t row = 0; row < vals_in.height(); ++row)
	{
		auto sign_col = 1;
		for(uint32_t col = 0; col < vals_in.width(); ++col)
		{
			input_buffer(col, row) = std::complex<double>{vals_in(col, row) * sign_row * sign_col};
			sign_col *= -1;
		}
		sign_row *= -1;
	}

	Model::BasicImage<Dft::Plan::sample_type> ret{vals_in.size()};
	m_plan_fwd.m_plan.execute(input_buffer.pixels().data(), ret.pixels().data());
	std::ranges::for_each(ret.pixels(), [area = vals_in.area()](auto& val) { return val /= area; });

	return ret;
}

Texpainter::Model::BasicImage<double> Texpainter::Generators::FourierTransform::
operator()(Span2d<Dft::Plan::sample_type const> vals_in)
{
	auto const w = vals_in.width();
	auto const h = vals_in.height();

	if(vals_in.size() != m_plan_bkwd.m_size) [[unlikely]]
		{
			m_plan_fwd.m_plan = Dft::Plan{vals_in.size(), Dft::Direction::Backward};
			m_plan_fwd.m_size = vals_in.size();
		}

	Model::BasicImage<std::complex<double>> output_buffer{w, h};
	m_plan_fwd.m_plan.execute(vals_in.data(), output_buffer.pixels().data());

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
