//@	{
//@	  "targets":[{"name":"engine.o", "type":"object"}]
//@	}

#include "dft/engine.hpp"

Texpainter::Dft::Engine const& Texpainter::Dft::engineInstance()
{
	static Engine engine;
	return engine;
}