//@	{
//@	 "targets":[{"name":"line_segment_renderer.o","type":"object", "pkgconfig_libs":["gtk+-3.0"]}]
//@	}

#include "./line_segment_renderer.hpp"

#include <gtk/gtk.h>

class Texpainter::Ui::LineSegmentRenderer::Impl : public LineSegmentRenderer
{
public:
	explicit Impl(Container& cnt): LineSegmentRenderer{*this}
	{
		auto widget = gtk_drawing_area_new();
		cnt.add(widget);
		m_handle            = GTK_DRAWING_AREA(widget);
	}

private:
	GtkDrawingArea* m_handle;
};

Texpainter::Ui::LineSegmentRenderer::LineSegmentRenderer(Container& cnt):
m_impl{new Impl{cnt}}
{

}

Texpainter::Ui::LineSegmentRenderer::~LineSegmentRenderer()
{
	delete m_impl;
}