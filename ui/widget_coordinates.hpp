//@	{"targets":[{"name":"widget_coordinates.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UI_WIDGETCOORDINATES_HPP
#define TEXPAINTER_UI_WIDGETCOORDINATES_HPP

#include "utils/vec_t.hpp"

namespace Texpainter::Ui
{
	class WidgetCoordinates
	{
	public:
		constexpr explicit WidgetCoordinates(double x, double y):m_value{x, y}{}
		
		constexpr double x() const
		{return m_value[0];}
		
		constexpr double y() const
		{return m_value[1];}
		
		
	private:
		vec2_t m_value;
	};
}

#endif
