//@	{
//@	"targets":[{"name":"constructible_from.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UTILS_CONSTRUCTIBLEFROM_HPP
#define TEXPAINTER_UTILS_CONSTRUCTIBLEFROM_HPP

namespace Texpainter
{
	template<class T, class... Args>
	concept ConstructibleFrom = requires
	{
		T(std::forward<Args>(std::declval<Args>())...);
	};
}

#endif