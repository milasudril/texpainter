//@	{"targets":[{"name":"widget_canvas_internal.hpp", "type":"include", "pkgconfig_libs":["gtk+-3.0"]}]}

#ifndef TEXPAINTER_UI_WIDGETCANVASINTERNAL_HPP
#define TEXPAINTER_UI_WIDGETCANVASINTERNAL_HPP

#include "./widget_coordinates.hpp"

#include <gtk/gtk.h>

#include <map>

GType widget_canvas_internal_get_type(void) G_GNUC_CONST;

namespace Texpainter::Ui
{
	class WidgetCanvasInternal: public GtkContainer
	{
	public:
		//	private:
		std::map<GtkWidget*, WidgetCoordinates> m_widgets;
	};

	WidgetCanvasInternal* widget_canvas_internal_new();

	inline WidgetCanvasInternal* asWidgetCanvas(GtkWidget* widget)
	{
		return G_TYPE_CHECK_INSTANCE_CAST(
		    widget, widget_canvas_internal_get_type(), WidgetCanvasInternal);
	}

	inline WidgetCanvasInternal* asWidgetCanvas(GtkContainer* widget)
	{
		return G_TYPE_CHECK_INSTANCE_CAST(
		    widget, widget_canvas_internal_get_type(), WidgetCanvasInternal);
	}

	inline WidgetCanvasInternal* asWidgetCanvas(GObject* widget)
	{
		return G_TYPE_CHECK_INSTANCE_CAST(
		    widget, widget_canvas_internal_get_type(), WidgetCanvasInternal);
	}
}

#endif