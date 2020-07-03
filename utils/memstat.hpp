//@	{
//@	 "targets":[{"name":"memstat.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"memstat.o", "rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UTILS_MEMSTAT_HPP
#define TEXPAINTER_UTILS_MEMSTAT_HPP

#include <cstddef>

namespace Texpainter
{
	size_t getAvailableRam();
}

#endif