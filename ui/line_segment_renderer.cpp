//@	{
//@	 "targets":[{"name":"line_segment_renderer.o","type":"object", "pkgconfig_libs":["gtk+-3.0"]}]
//@	}

#include "./line_segment_renderer.hpp"

#include <gtk/gtk.h>

class Texpainter::Ui::LineSegmentRenderer::Impl: public LineSegmentRenderer
{
public:
	explicit Impl(Container& cnt): LineSegmentRenderer{*this}
	{
		auto widget = gtk_drawing_area_new();
		g_signal_connect(G_OBJECT(widget), "draw", G_CALLBACK(draw_callback), this);
		cnt.add(widget);
		m_handle = GTK_DRAWING_AREA(widget);
	}

private:
	GtkDrawingArea* m_handle;

	static gboolean draw_callback(GtkWidget* widget, cairo_t* cr, gpointer self)
	{
		auto const w = static_cast<uint32_t>(gtk_widget_get_allocated_width(widget));
		auto const h = static_cast<uint32_t>(gtk_widget_get_allocated_height(widget));
		cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, .25);
		cairo_rectangle(cr, 0, 0, w, h);
		cairo_fill(cr);
		return FALSE;
	}
};

Texpainter::Ui::LineSegmentRenderer::LineSegmentRenderer(Container& cnt): m_impl{new Impl{cnt}} {}

Texpainter::Ui::LineSegmentRenderer::~LineSegmentRenderer() { delete m_impl; }