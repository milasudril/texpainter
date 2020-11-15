//@	{
//@	  "targets":[{"name":"engine.o", "type":"object"}]
//@	}

#include "dft/engine.hpp"

namespace
{
	thread_local Texpainter::Dft::Engine engine;
}

Texpainter::Dft::Engine const& Texpainter::Dft::engineInstance() { return engine; }