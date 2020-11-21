//@	{"targets":[{"name":"inplace_mutator.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_INPLACEMUTATOR_HPP
#define TEXPAINTER_UTILS_INPLACEMUTATOR_HPP

namespace Texpainter
{
	template<class F, class T>
	concept InplaceMutator = requires(F func, T& param)
	{
		noexcept(func(param));
		{
			func(param)
		}
		->std::same_as<bool>;
	};
}

#endif