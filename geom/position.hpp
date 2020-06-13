//@	{"targets":[{"name":"position.hpp","type":"include"}]}

#ifndef TEXPAINTER_GEOM_POSITION_HPP
#define TEXPAINTER_GEOM_POSITION_HPP

#include "utils/vec_t.hpp"

#include <utility>

namespace Texpainter::Geom
{
	class Position
	{
		public:
			using Scalar = decltype(std::declval<vec2_t>()[0]);

			Scalar x() const
				{return m_val[0];}

			Position& x(Scalar c)
				{
				m_val[0] = c;
				return *this;
				}

			Scalar y() const
				{return m_val[1];}

			Position& y(Scalar c)
				{
				m_val[1] = c;
				return *this;
				}

			Position& operator+=(vec2_t offset)
				{
				m_val+=offset;
				return *this;
				}

			Position& operator-=(vec2_t offset)
				{
				m_val-=offset;
				return *this;
				}

		private:
			vec2_t m_val;
	};

	inline Position operator-(Position p, vec2_t offset)
	{
		p-=offset;
		return p;
	}

	inline Position operator+(Position p, vec2_t offset)
	{
		p+=offset;
		return p;
	}
}

#endif
