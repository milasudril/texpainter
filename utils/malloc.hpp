//@	{"targets":[{"name":"malloc.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_MALLOC_HPP
#define TEXPAINTER_UTILS_MALLOC_HPP

#include "./memstat.hpp"

#include <cstdlib>

namespace Texpainter
{
	template<class T>
	T* allocMem(size_t n)
	{
		auto const N = n * sizeof(T);  // Here, it is assumed that there is no overflow.
		if(N > getAvailableRam() / 3)  // Divide by three to have some margin
		{
			throw std::runtime_error{
			    "The system as insufficient memory available to complete the action."};
		}

		return reinterpret_cast<T*>(malloc(N));
	}
}

#endif