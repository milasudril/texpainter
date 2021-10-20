//@	{
//@	  "targets":[{"name":"default_rng.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"default_rng.o", "rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UTILS_DEFAULTRNG_HPP
#define TEXPAINTER_UTILS_DEFAULTRNG_HPP


#include "pcg-cpp/include/pcg_random.hpp"

namespace Texpainter::DefaultRng
{
	class Engine:private pcg64
	{
	public:
		using pcg64::pcg64;
		using pcg64::operator();
		using pcg64::seed;
		using pcg64::min;
		using pcg64::max;
		using pcg64::state_type;
	};

	using State = Engine::state_type;

	Engine& engine();
	void seed(State val);

	inline State makeSeed()
	{
		auto& rng = engine();
		auto const a = State{rng()};
		auto const b = State{rng()} << 64;

		return a | b;
	}
}

#endif