//@	{"targets":[{"name":"compiler_flags.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_COMPILERFLAGS_HPP
#define TEXPAINTER_UTILS_COMPILERFLAGS_HPP

namespace Texpainter::CompilerFlags
{
#ifdef __FINITE_MATH_ONLY__
	constexpr bool HasNonFiniteMath = __FINITE_MATH_ONLY__ == 0;
#else
	constexpr bool HasNonFiniteMath = true;
#endif

#ifdef NDEBUG
	constexpr bool Debug = false;
#else
	constexpr bool Debug            = true;
#endif
}

#endif