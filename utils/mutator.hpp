//@	{"targets":[{"name":"mutator.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_MUTATOR_HPP
#define TEXPAINTER_UTILS_MUTATOR_HPP

namespace Texpainter
{
	template<class F, class T>
	concept Mutator = requires(F func, T& param)
	{
		noexcept(func(param));
		{
			func(param)
		}
		->std::same_as<bool>;
	};
}

#endif