//@	{"targets":[{"name":"screen_coordinates.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UI_SCREENCOORDINATES_HPP
#define TEXPAINTER_UI_SCREENCOORDINATES_HPP

#include "utils/vec_t.hpp"

namespace Texpainter::Ui
{
	class ScreenCoordinates
	{
	public:
		constexpr ScreenCoordinates() = default;

		constexpr explicit ScreenCoordinates(vec2_t v): m_value{v} {}

		constexpr explicit ScreenCoordinates(double x, double y): m_value{x, y} {}

		constexpr double x() const { return m_value[0]; }

		constexpr double y() const { return m_value[1]; }

		constexpr ScreenCoordinates& operator+=(vec2_t offset)
		{
			m_value += offset;
			return *this;
		}

		constexpr vec2_t value() const { return m_value; }

	private:
		vec2_t m_value;
	};

	inline constexpr ScreenCoordinates operator+(ScreenCoordinates a, vec2_t b) { return a += b; }

	inline constexpr vec2_t operator-(ScreenCoordinates a, ScreenCoordinates b)
	{
		return a.value() - b.value();
	}
}

#endif
