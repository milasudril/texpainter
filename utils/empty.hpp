//@	{"targets":[{"name":"empty.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_EMPTY_HPP
#define TEXPAINTER_UTILS_EMPTY_HPP

namespace Texpainter
{
	template<class T>
	struct Empty
	{
		using type = T;
	};
}

#endif