//@	{
//@	"targets":[{"name":"inherit_from.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UTILS_INHERITFROM_HPP
#define TEXPAINTER_UTILS_INHERITFROM_HPP

namespace Texpainter
{
	template<class FirstBase, class SecondBase>
	struct InheritFrom: FirstBase, SecondBase
	{
		template<class FirstBaseArgs, class... SecondBaseArgs>
		explicit InheritFrom(FirstBaseArgs&& first_args, SecondBaseArgs&&... second_args)
		    : FirstBase{std::forward<FirstBaseArgs>(first_args)}
		    , SecondBase{std::forward<SecondBaseArgs>(second_args)...}
		{
		}
	};
}

#endif