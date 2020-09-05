//@	{
//@	  "targets":[{"name":"default_rng.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"default_rng.o", "rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UTILS_DEFAULTRNG_HPP
#define TEXPAINTER_UTILS_DEFAULTRNG_HPP

#include "./polymorphic_rng.hpp"

namespace Texpainter::DefaultRng
{
	namespace detail
	{
		void engine(PolymorphicRng rng);
	}

	template<class Rng>
	void engine(Rng& rng)
	{
		detail::engine(PolymorphicRng{rng});
	}


	PolymorphicRng engine();
}

#endif