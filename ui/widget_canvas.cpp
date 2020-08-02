//@	{"targets":[{"name":"widget_canvas.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./widget_canvas.hpp"

#include <gtk/gtk.h>

class Texpainter::Ui::WidgetCanvas::Impl: private WidgetCanvas
{
public:
	Impl(Container& cnt);
	~Impl();

	void _add(GtkWidget* handle) noexcept
	{
		auto frame = GTK_FRAME(gtk_frame_new(nullptr));
		gtk_frame_set_shadow_type(frame, GTK_SHADOW_OUT);
		gtk_widget_set_events(GTK_WIDGET(frame),
		                      GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK
		                          | GDK_BUTTON_RELEASE_MASK);
		g_signal_connect(GTK_WIDGET(frame), "button-press-event", G_CALLBACK(button_press), this);
		g_signal_connect(
		    GTK_WIDGET(frame), "button-release-event", G_CALLBACK(button_relase), this);
		gtk_widget_set_margin_start(handle, 4);
		gtk_widget_set_margin_end(handle, 4);
		gtk_widget_set_margin_top(handle, 2);
		gtk_widget_set_margin_bottom(handle, 2);
		gtk_container_add(GTK_CONTAINER(frame), handle);
		gtk_fixed_put(GTK_FIXED(m_handle), GTK_WIDGET(frame), m_insert_loc[0], m_insert_loc[1]);
	}

	void _show() noexcept { gtk_widget_show_all(GTK_WIDGET(m_handle)); }

	void _sensitive(bool val) { gtk_widget_set_sensitive(GTK_WIDGET(m_handle), val); }

	void* _toplevel() const { return gtk_widget_get_toplevel(GTK_WIDGET(m_handle)); }

	void insertLocation(vec2_t loc) { m_insert_loc = loc; }

private:
	GtkFixed* m_handle;
	vec2_t m_insert_loc;

	GtkWidget* m_moving;
	vec2_t m_move_origin;

	static gboolean mouse_move(GtkWidget*, GdkEvent* event, gpointer user_data)
	{
		auto self = reinterpret_cast<Impl*>(user_data);
		if(self->m_moving != nullptr)
		{
			auto event_move = reinterpret_cast<GdkEventMotion const*>(event);
			auto w          = gtk_widget_get_allocated_width(self->m_moving);
			auto h          = gtk_widget_get_allocated_height(self->m_moving);

			auto dx = Texpainter::vec2_t{event_move->x_root, event_move->y_root}
			          - self->m_move_origin
			          - 0.5 * Texpainter::vec2_t{static_cast<double>(w), static_cast<double>(h)};
			dx = Texpainter::max(dx, vec2_t{0.0, 0.0});
			gtk_fixed_move(self->m_handle, self->m_moving, dx[0], dx[1]);
			return TRUE;
		}
		return FALSE;
	}

	static gboolean button_press(GtkWidget* widget, GdkEvent* event, gpointer user_data)
	{
		auto self      = reinterpret_cast<Impl*>(user_data);
		self->m_moving = widget;
		int x{};
		int y{};
		gdk_window_get_origin(gtk_widget_get_window(GTK_WIDGET(self->m_handle)), &x, &y);
		self->m_move_origin = Texpainter::vec2_t{static_cast<double>(x), static_cast<double>(y)};
		return FALSE;
	}

	static gboolean button_relase(GtkWidget* widget, GdkEvent* event, gpointer user_data)
	{
		auto self      = reinterpret_cast<Impl*>(user_data);
		self->m_moving = nullptr;
		return FALSE;
	}
};

Texpainter::Ui::WidgetCanvas::Impl::Impl(Container& cnt): WidgetCanvas{*this}
{
	auto widget = gtk_fixed_new();
	m_handle    = GTK_FIXED(widget);
	gtk_widget_set_events(GTK_WIDGET(m_handle),
	                      GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK
	                          | GDK_BUTTON_RELEASE_MASK);
	g_signal_connect(widget, "motion-notify-event", G_CALLBACK(mouse_move), this);
	cnt.add(widget);
	m_insert_loc = vec2_t{0.0, 0.0};
	m_moving     = nullptr;
}

Texpainter::Ui::WidgetCanvas::Impl::~Impl()
{
	m_impl = nullptr;
	gtk_widget_destroy(GTK_WIDGET(m_handle));
}

Texpainter::Ui::WidgetCanvas::WidgetCanvas(Container& cnt) { m_impl = new Impl(cnt); }

Texpainter::Ui::WidgetCanvas::~WidgetCanvas() { delete m_impl; }

Texpainter::Ui::WidgetCanvas& Texpainter::Ui::WidgetCanvas::add(void* handle)
{
	m_impl->_add(GTK_WIDGET(handle));
	return *this;
}

Texpainter::Ui::WidgetCanvas& Texpainter::Ui::WidgetCanvas::insertLocation(vec2_t loc)
{
	m_impl->insertLocation(loc);
	return *this;
}

Texpainter::Ui::WidgetCanvas& Texpainter::Ui::WidgetCanvas::show()
{
	m_impl->_show();
	return *this;
}

Texpainter::Ui::WidgetCanvas& Texpainter::Ui::WidgetCanvas::sensitive(bool val)
{
	m_impl->_sensitive(val);
	return *this;
}

void* Texpainter::Ui::WidgetCanvas::toplevel() const { return m_impl->_toplevel(); }
