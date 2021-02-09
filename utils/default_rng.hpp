//@	{
//@	  "targets":[{"name":"default_rng.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"default_rng.o", "rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UTILS_DEFAULTRNG_HPP
#define TEXPAINTER_UTILS_DEFAULTRNG_HPP


#include "pcg-cpp/include/pcg_random.hpp"

namespace Texpainter::DefaultRng
{
	using type = pcg64;

	type& engine();
	void seed(uint64_t val);
}

#endif