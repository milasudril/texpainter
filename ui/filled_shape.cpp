//@	{"targets":[{"name":"filled_shape.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./filled_shape.hpp"
#include "./container.hpp"

#include <gtk/gtk.h>

class Texpainter::Ui::FilledShape::Impl: private FilledShape
{
public:
	Impl(Container& cnt, PixelStore::RgbaValue color, double radius, vec2_t location);
	~Impl();

	ToplevelCoordinates location() const
	{
		int x{};
		int y{};
		auto widget = GTK_WIDGET(m_handle);
		gtk_widget_translate_coordinates(widget, gtk_widget_get_toplevel(widget), 0, 0, &x, &y);
		auto const w = static_cast<double>(gtk_widget_get_allocated_width(widget));
		auto const h = static_cast<double>(gtk_widget_get_allocated_height(widget));

		auto const size = vec2_t{w, h};

		return ToplevelCoordinates{static_cast<double>(x), static_cast<double>(y)}
		       + 0.5 * (size + size * m_loc);
	}

	void eventHandler(void* event_handler, Vtable const& vt)
	{
		r_eh = event_handler;
		m_vt = vt;
	}

private:
	void* r_eh;
	Vtable m_vt;

	PixelStore::RgbaValue m_color;
	double m_radius;
	vec2_t m_loc;
	bool m_completed;

	GtkDrawingArea* m_handle;

	static gboolean draw_callback(GtkWidget* widget, cairo_t* cr, gpointer data)
	{
		auto self            = reinterpret_cast<Impl*>(data);
		auto const color_g22 = PixelStore::RgbaValueG22(self->m_color);
		cairo_set_source_rgba(
		    cr, color_g22.red(), color_g22.green(), color_g22.blue(), color_g22.alpha());

		auto const w = gtk_widget_get_allocated_width(widget);
		auto const h = gtk_widget_get_allocated_height(widget);

		if(w < 2 && h > 1)  // It may happen that we have not yet computed the size
		{
			gtk_widget_queue_resize(widget);
			return FALSE;
		}

		if(!self->m_completed && self->r_eh != nullptr)
		{
			self->m_vt.on_completed(self->r_eh, *self);
			self->m_completed = true;
		}

		auto const size = vec2_t{static_cast<double>(w), static_cast<double>(h)};
		auto const O    = size / 2.0;
		auto const upper_left =
		    O + size * (self->m_loc - vec2_t{self->m_radius, self->m_radius}) / 2.0;
		auto const d = size * vec2_t{self->m_radius, self->m_radius};
		cairo_rectangle(cr, upper_left[0], upper_left[1], d[0], d[1]);
		cairo_fill(cr);

		return FALSE;
	}

	static void size_callback(GtkWidget* widget, GdkRectangle* allocation, gpointer)
	{
		auto const h = allocation->height;
		gtk_widget_set_size_request(widget, h, -1);
	}

	static gboolean clicked_callack(GtkWidget*, GdkEvent* e, gpointer self)
	{
		auto& obj         = *reinterpret_cast<Impl*>(self);
		auto event_button = reinterpret_cast<GdkEventButton const*>(e);
		if(obj.r_eh != nullptr && event_button->button == 1)
		{
			obj.m_vt.on_clicked(obj.r_eh, obj);
			return FALSE;
		}
		return TRUE;
	}

	static gboolean on_mouse_down(GtkWidget*, GdkEvent* e, gpointer self)
	{
		auto& obj         = *reinterpret_cast<Impl*>(self);
		auto event_button = reinterpret_cast<GdkEventButton const*>(e);
		if(obj.r_eh != nullptr && event_button->button == 1) { return FALSE; }
		return TRUE;
	}
};

Texpainter::Ui::FilledShape::FilledShape(Container& cnt,
                                         PixelStore::RgbaValue color,
                                         double radius,
                                         vec2_t location)
{
	m_impl = new Impl(cnt, color, radius, location);
}

Texpainter::Ui::FilledShape::~FilledShape() { delete m_impl; }

Texpainter::Ui::FilledShape::Impl::Impl(Container& cnt,
                                        PixelStore::RgbaValue color,
                                        double radius,
                                        vec2_t location)
    : FilledShape{*this}
    , m_color{color}
    , m_radius{radius}
    , m_loc{location}
    , m_completed{false}
{
	auto widget = gtk_drawing_area_new();
	g_signal_connect(G_OBJECT(widget), "draw", G_CALLBACK(draw_callback), this);
	g_signal_connect(G_OBJECT(widget), "size-allocate", G_CALLBACK(size_callback), this);
	g_signal_connect(G_OBJECT(widget), "button-release-event", G_CALLBACK(clicked_callack), this);
	g_signal_connect(G_OBJECT(widget), "button-press-event", G_CALLBACK(on_mouse_down), this);
	gtk_widget_add_events(widget, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);

	m_handle = GTK_DRAWING_AREA(widget);

	cnt.add(widget);
}

Texpainter::Ui::FilledShape::Impl::~Impl()
{
	m_impl = nullptr;
	gtk_widget_destroy(GTK_WIDGET(m_handle));
}

Texpainter::Ui::ToplevelCoordinates Texpainter::Ui::FilledShape::location() const
{
	return m_impl->location();
}

Texpainter::Ui::FilledShape& Texpainter::Ui::FilledShape::eventHandler(void* event_handler,
                                                                       Vtable const& vt)
{
	m_impl->eventHandler(event_handler, vt);
	return *this;
}
