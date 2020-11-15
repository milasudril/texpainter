//@	{
//@	  "targets":[{"name":"default_rng.o","type":"object"}]
//@	}

#include "./default_rng.hpp"

namespace
{
	thread_local pcg64 default_rng{};
}

void Texpainter::DefaultRng::seed(uint64_t value) { default_rng.seed(value); }

pcg64& Texpainter::DefaultRng::engine() { return default_rng; }
