//@	{"targets":[{"name":"numconv.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_NUMCONV_HPP
#define TEXPAINTER_UTILS_NUMCONV_HPP

#include <cstdlib>
#include <cstdio>
#include <optional>
#include <cerrno>
#include <limits>
#include <array>

namespace Texpainter
{
	inline std::array<char, 16> toArray(unsigned int x)
	{
		std::array<char, 16> ret{};
		sprintf(ret.data(), "%d", x);
		return ret;
	}

	inline std::optional<unsigned int> toInt(char const* str)
	{
		char* end;
		errno    = 0;
		auto val = strtol(str, &end, 10);
		if(errno != 0 || *end != '\0' || val > std::numeric_limits<unsigned int>::max())
		{ return std::optional<unsigned int>{}; }

		return static_cast<unsigned int>(val);
	}

	inline std::array<char, 16> toArray(float x)
	{
		std::array<char, 16> ret{};
		sprintf(ret.data(), "%.7e", x);
		return ret;
	}

	inline std::optional<float> toFloat(char const* str)
	{
		char* end;
		errno    = 0;
		auto val = strtof(str, &end);
		if(errno != 0 || *end != '\0') { return std::optional<float>{}; }

		return val;
	}
}

#endif