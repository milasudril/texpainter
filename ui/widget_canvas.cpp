//@	{"targets":[{"name":"widget_canvas.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./widget_canvas.hpp"

#include <gtk/gtk.h>

#include <map>
#include <cassert>


class Texpainter::Ui::WidgetCanvasDetail::Impl: private WidgetCanvasDetail
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
		    GTK_WIDGET(frame), "button-release-event", G_CALLBACK(button_release), this);
		gtk_widget_set_margin_start(handle, 4);
		gtk_widget_set_margin_end(handle, 4);
		gtk_widget_set_margin_top(handle, 4);
		gtk_widget_set_margin_bottom(handle, 4);
		gtk_container_add(GTK_CONTAINER(frame), handle);

		auto fixed_layout = GTK_FIXED(gtk_fixed_new());
		gtk_widget_set_events(GTK_WIDGET(fixed_layout),
		                      GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK
		                          | GDK_BUTTON_RELEASE_MASK);
		g_signal_connect(fixed_layout, "motion-notify-event", G_CALLBACK(mouse_move), this);
		g_signal_connect(fixed_layout, "button-press-event", G_CALLBACK(button_press_fixed), this);

		gtk_fixed_put(fixed_layout, GTK_WIDGET(frame), m_insert_loc[0], m_insert_loc[1]);
		m_floats[m_current_id]  = fixed_layout;
		m_clients[fixed_layout] = m_client_id;
		gtk_overlay_add_overlay(m_handle, GTK_WIDGET(fixed_layout));
		gtk_overlay_set_overlay_pass_through(m_handle, GTK_WIDGET(fixed_layout), TRUE);
	}

	void _show() noexcept { gtk_widget_show_all(GTK_WIDGET(m_handle)); }

	void _sensitive(bool val) { gtk_widget_set_sensitive(GTK_WIDGET(m_handle), val); }

	void* _toplevel() const { return gtk_widget_get_toplevel(GTK_WIDGET(m_handle)); }

	void insertLocation(vec2_t loc) { m_insert_loc = loc; }

	auto currentId() const { return m_current_id; }

	void nextId() { ++m_current_id; }

	void removeParentsFor(WidgetId id)
	{
		auto i = m_floats.find(id);
		assert(i != std::end(m_floats));
		m_clients.erase(i->second);
		gtk_widget_destroy(GTK_WIDGET(i->second));
		m_floats.erase(i);
	}

	void clientId(ClientId id) { m_client_id = id; }

private:
	GtkOverlay* m_handle;
	vec2_t m_insert_loc;

	GtkWidget* m_moving;
	vec2_t m_loc_init;
	vec2_t m_click_loc;
	WidgetId m_current_id;
	ClientId m_client_id;

	std::map<WidgetId, GtkFixed*> m_floats;
	std::map<GtkFixed*, ClientId> m_clients;

	static gboolean mouse_move(GtkWidget* widget, GdkEvent* event, gpointer user_data)
	{
		auto self = reinterpret_cast<Impl*>(user_data);
		if(self->m_moving != nullptr)
		{
			auto fixed_layout = GTK_FIXED(widget);
			auto event_move   = reinterpret_cast<GdkEventMotion const*>(event);
			auto loc_new =
			    self->m_loc_init
			    + (Texpainter::vec2_t{event_move->x_root, event_move->y_root} - self->m_click_loc);
			loc_new = Texpainter::max(loc_new, vec2_t{0.0, 0.0});
			gtk_fixed_move(fixed_layout, self->m_moving, loc_new[0], loc_new[1]);
			return TRUE;
		}
		return FALSE;
	}

	static gboolean button_press(GtkWidget* widget, GdkEvent* event, gpointer user_data)
	{
		auto self         = reinterpret_cast<Impl*>(user_data);
		auto fixed_layout = gtk_widget_get_ancestor(widget, GTK_TYPE_FIXED);
		gtk_overlay_reorder_overlay(self->m_handle, fixed_layout, -1);
		self->m_moving = widget;

		{
			GValue val_x{};
			g_value_init(&val_x, G_TYPE_INT);
			gtk_container_child_get_property(
			    GTK_CONTAINER(fixed_layout), self->m_moving, "x", &val_x);

			GValue val_y{};
			g_value_init(&val_y, G_TYPE_INT);
			gtk_container_child_get_property(
			    GTK_CONTAINER(fixed_layout), self->m_moving, "y", &val_y);

			self->m_loc_init = vec2_t{static_cast<double>(g_value_get_int(&val_x)),
			                          static_cast<double>(g_value_get_int(&val_y))};
		}

		{
			auto e = reinterpret_cast<GdkEventButton const*>(event);
			self->m_click_loc =
			    vec2_t{static_cast<double>(e->x_root), static_cast<double>(e->y_root)};
		}
		return FALSE;
	}

	static gboolean button_press_fixed(GtkWidget* widget, GdkEvent* event, gpointer user_data)
	{
		auto self = reinterpret_cast<Impl*>(user_data);
		auto i    = self->m_clients.find(GTK_FIXED(widget));
		assert(i != std::end(self->m_clients));
		printf("%zu\n", i->second.value());
		return FALSE;
	}

	static gboolean button_release(GtkWidget* widget, GdkEvent* event, gpointer user_data)
	{
		auto self      = reinterpret_cast<Impl*>(user_data);
		self->m_moving = nullptr;
		return FALSE;
	}
};

Texpainter::Ui::WidgetCanvasDetail::WidgetDeleter::WidgetDeleter(
    std::reference_wrapper<Impl> impl) noexcept
    : r_impl{impl}
    , m_id{r_impl.get().currentId()}
{
	r_impl.get().nextId();
}

void Texpainter::Ui::WidgetCanvasDetail::WidgetDeleter::do_cleanup() noexcept
{
	r_impl.get().removeParentsFor(m_id);
}


Texpainter::Ui::WidgetCanvasDetail::Impl::Impl(Container& cnt): WidgetCanvasDetail{*this}
{
	auto widget = gtk_overlay_new();
	m_handle    = GTK_OVERLAY(widget);
	cnt.add(widget);
	m_insert_loc = vec2_t{0.0, 0.0};
	m_moving     = nullptr;
}

Texpainter::Ui::WidgetCanvasDetail::Impl::~Impl()
{
	m_impl = nullptr;
	gtk_widget_destroy(GTK_WIDGET(m_handle));
}

Texpainter::Ui::WidgetCanvasDetail::WidgetCanvasDetail(Container& cnt) { m_impl = new Impl(cnt); }

Texpainter::Ui::WidgetCanvasDetail::~WidgetCanvasDetail() { delete m_impl; }

Texpainter::Ui::WidgetCanvasDetail& Texpainter::Ui::WidgetCanvasDetail::add(void* handle)
{
	m_impl->_add(GTK_WIDGET(handle));
	return *this;
}

Texpainter::Ui::WidgetCanvasDetail& Texpainter::Ui::WidgetCanvasDetail::insertLocation(vec2_t loc)
{
	m_impl->insertLocation(loc);
	return *this;
}

Texpainter::Ui::WidgetCanvasDetail& Texpainter::Ui::WidgetCanvasDetail::show()
{
	m_impl->_show();
	return *this;
}

Texpainter::Ui::WidgetCanvasDetail& Texpainter::Ui::WidgetCanvasDetail::sensitive(bool val)
{
	m_impl->_sensitive(val);
	return *this;
}

void* Texpainter::Ui::WidgetCanvasDetail::toplevel() const { return m_impl->_toplevel(); }

Texpainter::Ui::WidgetCanvasDetail& Texpainter::Ui::WidgetCanvasDetail::clientId(ClientId id)
{
	m_impl->clientId(id);
	return *this;
}