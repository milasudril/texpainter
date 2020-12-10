//@	{"targets":[{"name":"n_ary_transform.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_NARYTRANSFORM_HPP
#define TEXPAINTER_UTILS_NARYTRANSFORM_HPP

namespace Texpainter
{
	template<class Functor, class OutputIterator, class Input1, class... Inputs>
	OutputIterator transform(
	    Functor f, OutputIterator out, Input1 first1, Input1 last1, Inputs... inputs)
	{
		while(first1 != last1)
		{
			*out++ = f(*first1++, *inputs++...);
		}
		return out;
	}
}

#endif