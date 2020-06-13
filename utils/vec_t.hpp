//@	{"targets":[{"name":"vec_t.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_VECT_HPP
#define TEXPAINTER_UTILS_VECT_HPP

namespace Texpainter
{
	using vec4_t __attribute__((vector_size(16))) = float;
	using vec2_t __attribute__((vector_size(16))) = double;
}

#endif