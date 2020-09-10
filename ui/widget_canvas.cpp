//@	{"targets":[{"name":"widget_canvas.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./widget_canvas.hpp"
#include "./screen_coordinates.hpp"
#include "./toplevel_coordinates.hpp"

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
		switch(m_ins_mode)
		{
			case InsertMode::Fixed:
				gtk_widget_set_events(handle,
				                      GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK
				                          | GDK_BUTTON_RELEASE_MASK);
				g_signal_connect(handle, "button-release-event", G_CALLBACK(canvas_mouse_up), this);
				gtk_overlay_add_overlay(m_handle, handle);
				break;
			case InsertMode::Movable:
				auto frame = GTK_FRAME(gtk_frame_new(nullptr));
				gtk_frame_set_shadow_type(frame, GTK_SHADOW_OUT);
				gtk_widget_set_events(GTK_WIDGET(frame),
				                      GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK
				                          | GDK_BUTTON_RELEASE_MASK);
				g_signal_connect(
				    GTK_WIDGET(frame), "button-press-event", G_CALLBACK(button_press), this);
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
				g_signal_connect(
				    fixed_layout, "button-press-event", G_CALLBACK(button_press_fixed), this);

				gtk_fixed_put(fixed_layout, GTK_WIDGET(frame), m_insert_loc.x(), m_insert_loc.y());
				m_floats[m_client_id]   = fixed_layout;
				m_clients[fixed_layout] = m_client_id;

				gtk_overlay_add_overlay(m_handle, GTK_WIDGET(fixed_layout));
				gtk_overlay_set_overlay_pass_through(m_handle, GTK_WIDGET(fixed_layout), TRUE);
				break;
		}
	}

	void _show() noexcept { gtk_widget_show_all(GTK_WIDGET(m_handle)); }

	void _sensitive(bool val) { gtk_widget_set_sensitive(GTK_WIDGET(m_handle), val); }

	void* _toplevel() const { return gtk_widget_get_toplevel(GTK_WIDGET(m_handle)); }

	void insertLocation(WidgetCoordinates loc) { m_insert_loc = loc; }

	void removeParentsFor(ClientId id)
	{
		auto i = m_floats.find(id);
		assert(i != std::end(m_floats));
		m_clients.erase(i->second);
		gtk_widget_destroy(GTK_WIDGET(i->second));
		m_floats.erase(i);
	}

	void clientId(ClientId id) { m_client_id = id; }

	ClientId clientId() const { return m_client_id; }

	void eventHandler(void* event_handler, EventHandlerVtable const& vt)
	{
		r_eh = event_handler;
		m_vt = vt;
	}

	void insertMode(InsertMode mode) { m_ins_mode = mode; }

	WidgetCoordinates widgetLocation(ClientId client) const
	{
		auto i = m_floats.find(client);
		assert(i != std::end(m_floats));

		int x{};
		int y{};
		auto children = gtk_container_get_children(GTK_CONTAINER(i->second));
		auto frame    = GTK_WIDGET(children->data);
		g_list_free(children);

		gtk_widget_translate_coordinates(frame, GTK_WIDGET(m_handle), 0, 0, &x, &y);

		return WidgetCoordinates{static_cast<double>(x), static_cast<double>(y)};
	}


private:
	GtkOverlay* m_handle;
	WidgetCoordinates m_insert_loc;
	InsertMode m_ins_mode;

	GtkWidget* m_moving;
	WidgetCoordinates m_loc_init;
	ScreenCoordinates m_click_loc;
	ClientId m_client_id;
	void* r_eh;
	EventHandlerVtable m_vt;

	struct ClientIdCompare
	{
		bool operator()(ClientId a, ClientId b) const { return a.value() < b.value(); }
	};

	std::map<ClientId, GtkFixed*, ClientIdCompare> m_floats;
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
			    + (ScreenCoordinates{event_move->x_root, event_move->y_root} - self->m_click_loc);
			loc_new = max(loc_new, WidgetCoordinates{0.0, 0.0});
			gtk_fixed_move(fixed_layout, self->m_moving, loc_new.x(), loc_new.y());
			if(self->r_eh != nullptr)
			{
				if(auto i = self->m_clients.find(fixed_layout); i != std::end(self->m_clients))
				{ self->m_vt.on_move(self->r_eh, *self, loc_new, i->second); }
			}
			return TRUE;
		}
		return FALSE;
	}

	static gboolean button_press(GtkWidget* widget, GdkEvent* event, gpointer user_data)
	{
		auto e = reinterpret_cast<GdkEventButton const*>(event);
		if(e->button == 1)
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

				self->m_loc_init = WidgetCoordinates{static_cast<double>(g_value_get_int(&val_x)),
				                                     static_cast<double>(g_value_get_int(&val_y))};
			}

			{
				self->m_click_loc = ScreenCoordinates{static_cast<double>(e->x_root),
				                                      static_cast<double>(e->y_root)};
			}
			return TRUE;
		}
		return FALSE;
	}

	static gboolean button_press_fixed(GtkWidget* widget, GdkEvent* event, gpointer user_data)
	{
		auto self = reinterpret_cast<Impl*>(user_data);
		if(self->r_eh != nullptr)
		{
			if(auto i = self->m_clients.find(GTK_FIXED(widget)); i != std::end(self->m_clients))
			{
				auto e = reinterpret_cast<GdkEventButton const*>(event);
				self->m_vt.on_mouse_down(self->r_eh,
				                         *self,
				                         WidgetCoordinates{e->x, e->y},
				                         ScreenCoordinates{e->x_root, e->y_root},
				                         e->button,
				                         i->second);
			}
		}
		return FALSE;
	}

	static gboolean button_release(GtkWidget*, GdkEvent*, gpointer user_data)
	{
		auto self      = reinterpret_cast<Impl*>(user_data);
		self->m_moving = nullptr;
		return FALSE;
	}

	static void on_realized(GtkWidget*, gpointer user_data)
	{
		auto self = reinterpret_cast<Impl*>(user_data);
		if(self->r_eh != nullptr) { self->m_vt.on_realized(self->r_eh, *self); }
	}

	static gboolean canvas_mouse_move(GtkWidget* widget, GdkEvent* event, gpointer user_data)
	{
		auto self = reinterpret_cast<Impl*>(user_data);
		if(self->r_eh != nullptr)
		{
			auto event_move = reinterpret_cast<GdkEventMotion const*>(event);

			assert(GTK_OVERLAY(widget) == self->m_handle);

			// Manually compute correct coordinates for the event
			int screen_x{};
			int screen_y{};
			gdk_window_get_origin(gtk_widget_get_window(widget), &screen_x, &screen_y);
			auto delta =
			    ScreenCoordinates{event_move->x_root, event_move->y_root}
			    - ScreenCoordinates{static_cast<double>(screen_x), static_cast<double>(screen_y)};

			self->m_vt.on_move_canvas(self->r_eh, *self, ToplevelCoordinates{0.0, 0.0} + delta);
			return TRUE;
		}
		return FALSE;
	}
	static gboolean canvas_mouse_up(GtkWidget*, GdkEvent* event, gpointer user_data)
	{
		auto self = reinterpret_cast<Impl*>(user_data);
		if(self->r_eh != nullptr)
		{
			auto e = reinterpret_cast<GdkEventButton const*>(event);
			self->m_vt.on_mouse_up_canvas(self->r_eh,
			                              *self,
			                              WidgetCoordinates{e->x, e->y},
			                              ScreenCoordinates{e->x_root, e->y_root},
			                              e->button);
			return TRUE;
		}
		return FALSE;
	}
};

Texpainter::Ui::WidgetCanvasDetail::WidgetDeleter::WidgetDeleter(
    std::reference_wrapper<Impl> impl) noexcept
    : r_impl{&impl.get()}
    , m_id{r_impl->clientId()}
{
}

void Texpainter::Ui::WidgetCanvasDetail::WidgetDeleter::do_cleanup() noexcept
{
	if(r_impl != nullptr) { r_impl->removeParentsFor(m_id); }
}


Texpainter::Ui::WidgetCanvasDetail::Impl::Impl(Container& cnt)
    : WidgetCanvasDetail{*this}
    , m_insert_loc{0.0, 0.0}
    , m_ins_mode{InsertMode::Fixed}
{
	auto widget = gtk_overlay_new();
	m_handle    = GTK_OVERLAY(widget);
	gtk_widget_set_events(
	    widget, GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);
	gtk_widget_set_size_request(widget, 500, 300);
	g_signal_connect(widget, "motion-notify-event", G_CALLBACK(canvas_mouse_move), this);

	cnt.add(widget);
	m_moving = nullptr;
	r_eh     = nullptr;
	g_signal_connect(widget, "map", G_CALLBACK(on_realized), this);
}

Texpainter::Ui::WidgetCanvasDetail::Impl::~Impl()
{
	r_eh   = nullptr;
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

Texpainter::Ui::WidgetCanvasDetail& Texpainter::Ui::WidgetCanvasDetail::insertLocation(
    WidgetCoordinates loc)
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

void Texpainter::Ui::WidgetCanvasDetail::eventHandler(void* event_handler,
                                                      EventHandlerVtable const& vt)
{
	m_impl->eventHandler(event_handler, vt);
}

Texpainter::Ui::WidgetCanvasDetail& Texpainter::Ui::WidgetCanvasDetail::insertMode(InsertMode mode)
{
	m_impl->insertMode(mode);
	return *this;
}

Texpainter::Ui::WidgetCoordinates Texpainter::Ui::WidgetCanvasDetail::widgetLocation(
    ClientId id) const
{
	return m_impl->widgetLocation(id);
}