//@	{"targets":[{"name":"trivial.hpp", "type":"include"}]}

#ifndef TEXPAINTER_TRIVIAL_HPP
#define TEXPAINTER_TRIVIAL_HPP

#include <type_traits>

namespace Texpainter
{
	template <class T>
	concept Trivial = std::is_trivial_v<T>;
}

#endif
