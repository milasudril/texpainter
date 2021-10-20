//@	{
//@	  "targets":[{"name":"default_rng.o","type":"object"}]
//@	}

#include "./default_rng.hpp"

#include <random>

namespace
{
	thread_local Texpainter::DefaultRng::Engine default_rng{};
}

void Texpainter::DefaultRng::seed(uint64_t value) { default_rng.seed(value); }

Texpainter::DefaultRng::Engine& Texpainter::DefaultRng::engine() { return default_rng; }
