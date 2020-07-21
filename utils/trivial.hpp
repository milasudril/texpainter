//@	{"targets":[{"name":"trivial.hpp", "type":"include"}]}

#ifndef TEXPAINTER_TRIVIAL_HPP
#define TEXPAINTER_TRIVIAL_HPP

#include <type_traits>
#include <complex>

namespace Texpainter
{
	template<class T>
	concept Trivial =
	    std::is_trivial_v<
	        T> || std::is_same_v<std::complex<float>, T> || std::is_same_v<std::complex<double>, T>;
}

#endif
