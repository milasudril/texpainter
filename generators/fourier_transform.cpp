//@	{
//@	  "targets":[{"name":"fourier_transform.o", "type":"object", "pkgconfig_libs":["fftw3"]}]
//@	}

#include "./fourier_transform.hpp"

#include <cstring>

Texpainter::Dft::Engine Texpainter::Generators::FourierTransform::s_engine;

Texpainter::PixelStore::BasicImage<std::complex<double>> Texpainter::Generators::FourierTransform::
operator()(Span2d<double const> vals_in)
{
	auto sign_row = 1;
	PixelStore::BasicImage<std::complex<double>> input_buffer{vals_in.size()};
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

	PixelStore::BasicImage<std::complex<double>> ret{vals_in.size()};
	s_engine.run<Dft::Direction::Forward>(
	    vals_in.size(), input_buffer.pixels().data(), ret.pixels().data());
	std::ranges::for_each(ret.pixels(), [area = vals_in.area()](auto& val) { return val /= area; });

	return ret;
}

Texpainter::PixelStore::BasicImage<double> Texpainter::Generators::FourierTransform::operator()(
    Span2d<std::complex<double> const> vals_in)
{
	auto const w = vals_in.width();
	auto const h = vals_in.height();

	PixelStore::BasicImage<std::complex<double>> output_buffer{w, h};
	s_engine.run<Dft::Direction::Backward>(
	    vals_in.size(), vals_in.data(), output_buffer.pixels().data());

	Texpainter::PixelStore::BasicImage<double> ret{w, h};
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
