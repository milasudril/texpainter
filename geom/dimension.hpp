//@	{"targets":[{"name":"dimension.hpp","type":"include"}]}

#ifndef TEXPAINTER_GEOM_DIMENSION_HPP
#define TEXPAINTER_GEOM_DIMENSION_HPP

#include "utils/vec_t.hpp"

#include <utility>

namespace Texpainter::Geom
{
	class Dimension
	{
	public:
		using Scalar = decltype(std::declval<vec2_t>()[0]);

		Scalar width() const
		{
			return m_val[0];
		}

		Dimension& width(Scalar c)
		{
			m_val[0] = c;
			return *this;
		}

		Scalar height() const
		{
			return m_val[1];
		}

		Dimension& height(Scalar c)
		{
			m_val[1] = c;
			return *this;
		}

		Dimension& operator*=(vec2_t scale)
		{
			m_val *= scale;
			return *this;
		}

		auto offsetVector() const
		{
			return m_val;
		}

	private:
		vec2_t m_val;
	};

	inline Dimension operator*(Dimension d, vec2_t scale)
	{
		d *= scale;
		return d;
	}

	inline Dimension operator*(vec2_t scale, Dimension d)
	{
		d *= scale;
		return d;
	}
}

#endif
