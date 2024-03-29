//@	{
//@	 "targets":[{"name":"line_segment_renderer.o","type":"object", "pkgconfig_libs":["gtk+-3.0"]}]
//@	}

#include "./line_segment_renderer.hpp"
#include "./widget_coordinates.hpp"

#include <gtk/gtk.h>

#include <vector>
#include <algorithm>

class Texpainter::Ui::LineSegmentRenderer::Impl: public LineSegmentRenderer
{
public:
	explicit Impl(Container& cnt): LineSegmentRenderer{*this}, m_offset{0.0, 0.0}
	{
		auto widget = gtk_drawing_area_new();
		g_signal_connect(G_OBJECT(widget), "draw", G_CALLBACK(draw_callback), this);
		g_signal_connect(G_OBJECT(widget), "realize", G_CALLBACK(realized), this);
		cnt.add(widget);
		m_handle = GTK_DRAWING_AREA(widget);
	}

	~Impl()
	{
		m_impl = nullptr;
		gtk_widget_destroy(GTK_WIDGET(m_handle));
	}

	void lineSegments(std::span<std::pair<ToplevelCoordinates, ToplevelCoordinates> const> segs)
	{
		std::vector<std::pair<ToplevelCoordinates, ToplevelCoordinates>> res;
		std::ranges::copy(segs, std::back_inserter(res));
		m_segs = std::move(res);
		gtk_widget_queue_draw(GTK_WIDGET(m_handle));
	}

	void renderOffset(vec2_t x)
	{
		m_offset = x;
		gtk_widget_queue_draw(GTK_WIDGET(m_handle));
	}

private:
	vec2_t m_offset;
	std::vector<std::pair<ToplevelCoordinates, ToplevelCoordinates>> m_segs;

	GtkDrawingArea* m_handle;

	static gboolean draw_callback(GtkWidget* widget, cairo_t* cr, gpointer obj)
	{
		int x{};
		int y{};
		cairo_set_source_rgba(cr, 0.72, 0.72, 0.72, 1.0);
		cairo_set_line_width(cr, 2.0);
		gtk_widget_translate_coordinates(widget, gtk_widget_get_toplevel(widget), 0, 0, &x, &y);
		auto self = reinterpret_cast<Impl*>(obj);
		std::ranges::for_each(
		    self->m_segs,
		    [cr,
		     O = ToplevelCoordinates{static_cast<double>(x), static_cast<double>(y)}
		         + self->m_offset](auto const& vals) {
			    auto a = vals.first - O;
			    auto b = vals.second - O;
			    cairo_move_to(cr, a[0], a[1]);
			    cairo_line_to(cr, b[0], b[1]);
			    cairo_stroke(cr);
		    });

		return FALSE;
	}

	static void realized(GtkWidget* widget, gpointer*)
	{
		auto window = gtk_widget_get_window(widget);
		gdk_window_set_pass_through(window, TRUE);
	}
};

Texpainter::Ui::LineSegmentRenderer::LineSegmentRenderer(Container& cnt): m_impl{new Impl{cnt}} {}

Texpainter::Ui::LineSegmentRenderer::~LineSegmentRenderer() { delete m_impl; }

Texpainter::Ui::LineSegmentRenderer& Texpainter::Ui::LineSegmentRenderer::lineSegments(
    std::span<std::pair<ToplevelCoordinates, ToplevelCoordinates> const> segs)
{
	m_impl->lineSegments(segs);
	return *this;
}

Texpainter::Ui::LineSegmentRenderer& Texpainter::Ui::LineSegmentRenderer::renderOffset(vec2_t x)
{
	m_impl->renderOffset(x);
	return *this;
}