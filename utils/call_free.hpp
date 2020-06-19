//@	{"targets":[{"name":"memblock.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_CALLFREE_HPP
#define TEXPAINTER_UTILS_CALLFREE_HPP

#include <cstdlib>

namespace Texpainter
{
	struct CallFree
	{
		void operator()(void* buffer)
		{
			free(buffer);
		}
	};
}		
#endif
