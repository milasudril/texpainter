//@	{
//@	 "targets":[{"name":"minmax_filter.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"minmax_filter.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_ROLLINGRANKFILTER_MINMAXFILTER_HPP
#define TEXPAINTER_ROLLINGRANKFILTER_MINMAXFILTER_HPP

#include "utils/span_2d.hpp"

#include <cstdint>

namespace Texpainter::RollingRankFilter
{
	void minmaxFilter(Span2d<float const> src,
	                  Span2d<int8_t const> mask,
	                  Span2d<float> min,
	                  Span2d<float> max);
}

#endif