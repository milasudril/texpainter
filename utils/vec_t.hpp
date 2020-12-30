//@	{
//@	 "targets":[{"name":"vec_t.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"vec_t.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UTILS_VECT_HPP
#define TEXPAINTER_UTILS_VECT_HPP

#include "./compiler_flags.hpp"
#include "./to_string.hpp"

#include "libenum/empty.hpp"

#include <nlohmann/json.hpp>

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

	constexpr float dot(vec4_t a, vec4_t b)
	{
		a *= b;
		return a[0] + a[1] + a[2] + a[3];
	}

	constexpr float length(vec4_t v) { return sqrt(dot(v, v)); }

	constexpr vec2_t transform(vec2_t v, vec2_t x_vec, vec2_t y_vec)
	{
		return vec2_t{dot(v, x_vec), dot(v, y_vec)};
	}

	constexpr auto min(vec4i_t a, vec4i_t b) { return a < b ? a : b; }

	constexpr auto min(vec4_t a, vec4_t b) { return a < b ? a : b; }

	constexpr auto max(vec4i_t a, vec4i_t b) { return a > b ? a : b; }

	constexpr auto max(vec4_t a, vec4_t b) { return a > b ? a : b; }

	constexpr auto max(vec2_t a, vec2_t b) { return a > b ? a : b; }


	constexpr vec2_t signum(vec2_t x)
	{
		auto const mask        = x < 0;
		auto const mask_double = vec2_t{static_cast<double>(mask[0]), static_cast<double>(mask[1])};
		return mask_double * vec2_t{1.0, 1.0} + (1.0 + mask_double) * vec2_t{1.0, 1.0};
	}

	constexpr vec4_t abs(vec4_t a) { return a < vec4_t{0.0f, 0.0f, 0.0f, 0.0f} ? -a : a; }

	inline std::string toString(vec2_t v)
	{
		auto ret = toString(v[0]);
		ret += " ";
		ret += toString(v[1]);
		return ret;
	}

	inline std::string toString(vec4_t v)
	{
		auto ret = std::string{};
		ret += Texpainter::toString(v[0]);
		ret += " ";
		ret += Texpainter::toString(v[1]);
		ret += " ";
		ret += Texpainter::toString(v[2]);
		ret += " ";
		ret += Texpainter::toString(v[3]);
		return ret;
	}

	Texpainter::vec4_t fromString(Enum::Empty<vec4_t>, std::string const& str);

	template<int dummy = 0>
	constexpr auto chooseValIfInRange(vec4_t val,
	                                  vec4_t if_too_small,
	                                  vec4_t if_too_large,
	                                  vec4_t if_nan) requires(CompilerFlags::HasNonFiniteMath)
	{
		return val < vec4_t{0.0f, 0.0f, 0.0f, 0.0f}
		           ? if_too_small
		           : (val > vec4_t{1.0f, 1.0f, 1.0f, 1.0f} ? if_too_large
		                                                   : ((val != val) ? if_nan : val));
	}

	constexpr auto chooseValIfInRange(vec4_t val, vec4_t if_too_small, vec4_t if_too_large)
	{
		return val < vec4_t{0.0f, 0.0f, 0.0f, 0.0f}
		           ? if_too_small
		           : (val > vec4_t{1.0f, 1.0f, 1.0f, 1.0f}) ? if_too_large : val;
	}

}

namespace nlohmann
{
	template<>
	struct adl_serializer<Texpainter::vec2_t>
	{
		static void to_json(json& j, Texpainter::vec2_t value) { j = Texpainter::toString(value); }

		static void from_json(const json&, Texpainter::vec2_t)
		{
			// TODO: same thing, but with the "from_json" method
		}
	};
}

#endif