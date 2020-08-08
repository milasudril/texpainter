//@	{"targets":[{"name":"widget_coordinates.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UI_WIDGETCOORDINATES_HPP
#define TEXPAINTER_UI_WIDGETCOORDINATES_HPP

#include "utils/vec_t.hpp"

namespace Texpainter::Ui
{
	class WidgetCoordinates
	{
	public:
		constexpr WidgetCoordinates() = default;

		constexpr explicit WidgetCoordinates(vec2_t val): m_value{val} {}

		constexpr explicit WidgetCoordinates(double x, double y): m_value{x, y} {}

		constexpr double x() const { return m_value[0]; }

		constexpr double y() const { return m_value[1]; }

		constexpr WidgetCoordinates& operator+=(vec2_t offset)
		{
			m_value += offset;
			return *this;
		}

		constexpr vec2_t value() const { return m_value; }


	private:
		vec2_t m_value;
	};

	inline constexpr WidgetCoordinates operator+(WidgetCoordinates a, vec2_t offset)
	{
		return a += offset;
	}

	constexpr WidgetCoordinates max(WidgetCoordinates a, WidgetCoordinates b)
	{
		return WidgetCoordinates{Texpainter::max(a.value(), b.value())};
	}
}

#endif
