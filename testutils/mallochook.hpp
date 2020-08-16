//@	{
//@	 "targets":[{"name":"mallochook.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"mallochook.o", "rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_TESTUTILS_MALLOCHOOK_HPP
#define TEXPAINTER_TESTUTILS_MALLOCHOOK_HPP

#include <cstdlib>

namespace Testutils::MallocHook
{
	size_t blockSize(void const* ptr);
	void init();
	void disarm();
	size_t size();
};

#endif