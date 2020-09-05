//@	{
//@	  "targets":[{"name":"default_rng.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"default_rng.o", "rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UTILS_DEFAULTRNG_HPP
#define TEXPAINTER_UTILS_DEFAULTRNG_HPP

#include "./polymorphic_rng.hpp"

namespace Texpainter::DefaultRng
{
	template<class Rng>
	void engine(Rng& rng)
	{
		engine(PolymorphicRng{rng});
	}

	void engine(PolymorphicRng rng);

	PolymorphicRng engine();
}

#endif