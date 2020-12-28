//@	{"targets":[{"name":"to_string.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_TOSTRING_HPP
#define TEXPAINTER_UTILS_TOSTRING_HPP

#include "./numconv.hpp"

#include <string>

namespace Texpainter
{
	template<class T>
	inline std::string toString(T const& val)
	{
		return std::to_string(val);
	}

	inline std::string const& toString(std::string const& val) { return val; }

	std::string const& toString(std::string const&&) = delete;

	inline std::string toString(float x) { return toArray(x).data(); }

	std::string toString(double);
}

#endif