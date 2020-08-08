//@	{"targets":[{"name":"toplevel_coordinates.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UI_TOPLEVELCOORDINATES_HPP
#define TEXPAINTER_UI_TOPLEVELCOORDINATES_HPP

#include "utils/vec_t.hpp"

namespace Texpainter::Ui
{
	class ToplevelCoordinates
	{
	public:
		ToplevelCoordinates() = default;
		constexpr explicit ToplevelCoordinates(double x, double y): m_value{x, y} {}

		constexpr double x() const { return m_value[0]; }

		constexpr double y() const { return m_value[1]; }

		constexpr ToplevelCoordinates& operator+=(vec2_t offset)
		{
			m_value += offset;
			return *this;
		}

		constexpr vec2_t value() const { return m_value; }

	private:
		vec2_t m_value;
	};

	inline constexpr ToplevelCoordinates operator+(ToplevelCoordinates a, vec2_t b)
	{
		return a += b;
	}

	inline constexpr vec2_t operator-(ToplevelCoordinates a, ToplevelCoordinates b)
	{
		return a.value() - b.value();
	}
}

#endif
