//@	{
//@	"targets":[{"name":"inherit_from.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UTILS_INHERITFROM_HPP
#define TEXPAINTER_UTILS_INHERITFROM_HPP

#include <utility>

namespace Texpainter
{
	template<class First, class Second>
	struct InheritFrom:public First, Second
	{
		template<class ... SecondClassArgs, class FirstClassArg>
		explicit InheritFrom(SecondClassArgs&& ... second, FirstClassArg&& first):
			 First{std::forward<First>(first)}
			,Second{std::forward<SecondClassArgs>(second)...}
		{}
	};
}

#endif