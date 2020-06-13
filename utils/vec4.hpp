//@	{"targets":[{"name":"vec4.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_VEC4_HPP
#define TEXPAINTER_UTILS_VEC4_HPP

namespace Texpainter
{
	using vec4_t __attribute__((vector_size(16))) = float;
}

#endif