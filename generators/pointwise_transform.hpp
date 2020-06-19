//@	{"targets":[{"name":"pointwise_transform.hpp", "type":"include"}]}

#ifndef TEXPAINTER_GENERATORS_POINTWISETRANSFORM_HPP
#define TEXPAINTER_GENERATORS_POINTWISETRANSFORM_HPP

#include "model/image.hpp"

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
		auto operator()(Span2d<T> in)
		{
			Model::BasicImage<std::remove_const_t<T>> ret{in.size()};
			transform(in, ret.pixels(), m_f);
			return ret;
		}

	private:
		Function m_f;
	};
}

#endif