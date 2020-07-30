//@	{
//@	  "targets":[{"name":"fourier_transform.hpp", "type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"fourier_transform.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_GENERATORS_FOURIERTRANSFORM_HPP
#define TEXPAINTER_GENERATORS_FOURIERTRANSFORM_HPP

#include "pixel_store/image.hpp"
#include "dft/engine.hpp"

namespace Texpainter::Generators
{
	class FourierTransform
	{
	public:
		PixelStore::BasicImage<std::complex<double>> operator()(Span2d<double const> vals_in);

		PixelStore::BasicImage<double> operator()(Span2d<std::complex<double> const> vals_in);

	private:
		static Dft::Engine s_engine;
	};
}

#endif