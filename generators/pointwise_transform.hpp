//@	{"targets":[{"name":"pointwise_transform.hpp", "type":"include"}]}

#ifndef TEXPAINTER_GENERATORS_POINTWISETRANSFORM_HPP
#define TEXPAINTER_GENERATORS_POINTWISETRANSFORM_HPP

#include <random>
#include <algorithm>

namespace Texpainter::Generators
{
	template<class Function>
	class PointwiseTransform
	{
	public:
		explicit PointwiseTransform(Function&& f): m_f{std::move(f)}
		{
		}

		template<class T>
		Model::BasicImage<T> operator()(Span2d<T> in)
		{
			Model::BasicImage<T> ret{in.size()};
			transform(in, ret.pixels(), m_f);
			return ret;
		}

	private:
		Function m_f;
	};
}

#endif