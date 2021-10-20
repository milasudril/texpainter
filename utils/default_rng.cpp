//@	{
//@	  "targets":[{"name":"default_rng.o","type":"object"}]
//@	}

#include "./default_rng.hpp"

#include <random>

namespace
{
	thread_local Texpainter::DefaultRng::Engine default_rng{};
}

Texpainter::DefaultRng::Engine& Texpainter::DefaultRng::engine() { return default_rng; }
