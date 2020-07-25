//@	{"targets":[{"name":"vec_t.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_VECT_HPP
#define TEXPAINTER_UTILS_VECT_HPP

#include <cmath>
#include <string>

namespace Texpainter
{
	using vec4_t __attribute__((vector_size(16)))        = float;
	using vec4i_t __attribute__((vector_size(16)))       = int;
	using vec4_double_t __attribute__((vector_size(32))) = double;
	using vec2_t __attribute__((vector_size(16)))        = double;

	constexpr double dot(vec2_t a, vec2_t b)
	{
		a *= b;
		return a[0] + a[1];
	}

	constexpr double length(vec2_t v) { return sqrt(dot(v, v)); }

	constexpr vec2_t transform(vec2_t v, vec2_t x_vec, vec2_t y_vec)
	{
		return vec2_t{dot(v, x_vec), dot(v, y_vec)};
	}

	constexpr auto min(vec4i_t a, vec4i_t b) { return a < b ? a : b; }

	constexpr auto min(vec4_t a, vec4_t b) { return a < b ? a : b; }

	constexpr vec2_t signum(vec2_t x)
	{
		auto const mask        = x < 0;
		auto const mask_double = vec2_t{static_cast<double>(mask[0]), static_cast<double>(mask[1])};
		return mask_double * vec2_t{1.0, 1.0} + (1.0 + mask_double) * vec2_t{1.0, 1.0};
	}

	inline std::string toString(vec2_t v)
	{
		auto ret = std::string{"("};
		ret += std::to_string(v[0]);
		ret += ", ";
		ret += std::to_string(v[1]);
		ret += ")";

		return ret;
	}
}

#endif