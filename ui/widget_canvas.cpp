//@	{"targets":[{"name":"widget_canvas.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./widget_canvas.hpp"
#include "./widget_canvas_internal.hpp"
#include "./screen_coordinates.hpp"
#include "./toplevel_coordinates.hpp"

#include "utils/size_2d.hpp"

#include <gtk/gtk.h>

#include <map>
#include <cassert>
#include <algorithm>

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
			{
				gtk_overlay_add_overlay(m_handle, handle);
				gtk_overlay_set_overlay_pass_through(m_handle, handle, TRUE);
				break;
			}

			case InsertMode::Movable:
			{
				auto frame = GTK_FRAME(gtk_frame_new(nullptr));
				gtk_widget_set_events(GTK_WIDGET(frame),
				                      GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK
				                          | GDK_BUTTON_RELEASE_MASK);
				g_signal_connect(
				    GTK_WIDGET(frame), "button-press-event", G_CALLBACK(button_press), this);
				g_signal_connect(
				    GTK_WIDGET(frame), "button-release-event", G_CALLBACK(button_release), this);

				gtk_frame_set_shadow_type(frame, GTK_SHADOW_OUT);
				gtk_widget_set_margin_start(handle, 4);
				gtk_widget_set_margin_end(handle, 4);
				gtk_widget_set_margin_top(handle, 4);
				gtk_widget_set_margin_bottom(handle, 4);
				gtk_container_add(GTK_CONTAINER(frame), handle);
				m_floats[m_client_id] = frame;
				m_clients[frame]      = m_client_id;
				m_widgets->insert(GTK_WIDGET(frame), m_insert_loc);
				break;
			}
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
		updateCanvasSize();
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

		return widgetLocationAndSize(i->second).first;
	}

	vec2_t viewportOffset() const
	{
		auto x_adj = gtk_adjustment_get_value(gtk_scrolled_window_get_hadjustment(m_root));
		auto y_adj = gtk_adjustment_get_value(gtk_scrolled_window_get_vadjustment(m_root));

		return vec2_t{x_adj, y_adj};
	}

	void updateCanvasSize()
	{
		if(m_clients.size() != 0)
		{
			auto res = calculateCanvasSize();
			gtk_widget_set_size_request(
			    GTK_WIDGET(m_handle), res.second.width(), res.second.height());

			auto x_adj = gtk_scrolled_window_get_hadjustment(m_root);
			auto y_adj = gtk_scrolled_window_get_vadjustment(m_root);

			gtk_adjustment_set_upper(x_adj, res.second.width() + 16);   // Compensate for scrollbars
			gtk_adjustment_set_upper(y_adj, res.second.height() + 16);  // Compensate for scrollbars
		}
	}

private:
	GtkScrolledWindow* m_root;
	GtkOverlay* m_handle;
	WidgetCanvasInternal* m_widgets;
	GtkWidget* m_bottom;
	WidgetCoordinates m_insert_loc;
	InsertMode m_ins_mode;

	GtkFrame* m_moving;
	WidgetCoordinates m_loc_init;
	ScreenCoordinates m_click_loc;
	ClientId m_client_id;
	void* r_eh;
	EventHandlerVtable m_vt;

	struct ClientIdCompare
	{
		bool operator()(ClientId a, ClientId b) const { return a.value() < b.value(); }
	};

	std::map<ClientId, GtkFrame*, ClientIdCompare> m_floats;
	std::map<GtkFrame*, ClientId> m_clients;

	std::pair<WidgetCoordinates, vec2_t> widgetLocationAndSize(GtkFrame* frame) const
	{
		int x{};
		int y{};
		auto const w = gtk_widget_get_allocated_width(GTK_WIDGET(frame));
		auto const h = gtk_widget_get_allocated_height(GTK_WIDGET(frame));

		gtk_widget_translate_coordinates(GTK_WIDGET(frame), GTK_WIDGET(m_handle), 0, 0, &x, &y);

		return std::pair{WidgetCoordinates{static_cast<double>(x), static_cast<double>(y)},
		                 vec2_t{static_cast<double>(w), static_cast<double>(h)}};
	}

	std::pair<GtkFrame*, Size2d> calculateCanvasSize() const
	{
		auto i      = std::ranges::max_element(m_clients, [this](auto const& a, auto const& b) {
            auto box_a    = widgetLocationAndSize(a.first);
            auto box_b    = widgetLocationAndSize(b.first);
            auto corner_a = box_a.first + box_a.second;
            auto corner_b = box_b.first + box_b.second;

            if(corner_a.x() < corner_b.x()) { return true; }

            if(corner_a.x() == corner_b.x()) { return corner_b.y() < corner_b.y(); }

            return false;
        });
		auto ret    = widgetLocationAndSize(i->first);
		auto corner = ret.first + ret.second;
		return std::pair{
		    i->first, Size2d{static_cast<uint32_t>(corner.x()), static_cast<uint32_t>(corner.y())}};
	}

	void scrollIntoView(GtkFrame* container)
	{
		auto size_loc = widgetLocationAndSize(container);
		auto x_adj    = gtk_scrolled_window_get_hadjustment(m_root);
		auto y_adj    = gtk_scrolled_window_get_vadjustment(m_root);

		gtk_adjustment_set_value(x_adj, size_loc.first.x());
		gtk_adjustment_set_value(y_adj, size_loc.first.y());
	}

	static gboolean widgets_mouse_move(GtkWidget* widget, GdkEvent* event, gpointer user_data)
	{
		auto self = reinterpret_cast<Impl*>(user_data);
		if(self->m_moving != nullptr)
		{
			auto fixed_layout = asWidgetCanvasInternal(widget);
			auto event_move   = reinterpret_cast<GdkEventMotion const*>(event);
			auto loc_new =
			    self->m_loc_init
			    + (ScreenCoordinates{event_move->x_root, event_move->y_root} - self->m_click_loc);
			loc_new = max(loc_new, WidgetCoordinates{0.0, 0.0});
			fixed_layout->move(GTK_WIDGET(self->m_moving), loc_new);
			if(self->r_eh != nullptr)
			{
				if(auto i = self->m_clients.find(self->m_moving); i != std::end(self->m_clients))
				{ self->m_vt.on_move(self->r_eh, *self, loc_new, i->second); }
			}
			return FALSE;
		}
		return FALSE;
	}

	static gboolean button_press(GtkWidget* widget, GdkEvent* event, gpointer user_data)
	{
		auto e    = reinterpret_cast<GdkEventButton const*>(event);
		auto self = reinterpret_cast<Impl*>(user_data);
		if(e->button == 1)
		{
			self->m_widgets->toFront(widget);
			self->m_moving   = GTK_FRAME(widget);
			self->m_loc_init = self->m_widgets->location(widget);
			self->m_click_loc =
			    ScreenCoordinates{static_cast<double>(e->x_root), static_cast<double>(e->y_root)};
			return FALSE;
		}

		if(self->r_eh != nullptr)
		{
			if(auto i = self->m_clients.find(GTK_FRAME(widget)); i != std::end(self->m_clients))
			{
				auto e = reinterpret_cast<GdkEventButton const*>(event);
				self->m_vt.on_mouse_down(self->r_eh,
				                         *self,
				                         self->m_widgets->location(widget) + vec2_t{e->x, e->y},
				                         ScreenCoordinates{e->x_root, e->y_root},
				                         e->button,
				                         i->second);
			}
		}
		return FALSE;
	}

	static gboolean button_release(GtkWidget*, GdkEvent*, gpointer user_data)
	{
		auto self = reinterpret_cast<Impl*>(user_data);
		if(self->m_moving != nullptr)
		{
			self->updateCanvasSize();
			self->scrollIntoView(self->m_moving);
			self->m_moving = nullptr;
		}
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
			int loc_x{};
			int loc_y{};
			gtk_widget_translate_coordinates(
			    widget, gtk_widget_get_toplevel(widget), 0, 0, &loc_x, &loc_y);

			auto offset = vec2_t{static_cast<double>(loc_x), static_cast<double>(loc_y)};
			self->m_vt.on_move_canvas(
			    self->r_eh, *self, ToplevelCoordinates{event_move->x, event_move->y} + offset);
			return FALSE;
		}
		return FALSE;
	}

	static gboolean canvas_mouse_down(GtkWidget*, GdkEvent* event, gpointer user_data)
	{
		auto self = reinterpret_cast<Impl*>(user_data);
		if(self->r_eh != nullptr)
		{
			auto e = reinterpret_cast<GdkEventButton const*>(event);
			self->m_vt.on_mouse_down_canvas(self->r_eh,
			                                *self,
			                                WidgetCoordinates{e->x, e->y},
			                                ScreenCoordinates{e->x_root, e->y_root},
			                                e->button);
			return FALSE;
		}
		return FALSE;
	}

	static void canvas_viewport_moved(GtkAdjustment*, gpointer user_data)
	{
		auto self = reinterpret_cast<Impl*>(user_data);
		if(self->r_eh != nullptr) { self->m_vt.on_viewport_moved(self->r_eh, *self); }
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
	m_root      = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(nullptr, nullptr));
	auto widget = gtk_overlay_new();
	gtk_container_add(GTK_CONTAINER(m_root), widget);
	m_handle   = GTK_OVERLAY(widget);
	auto frame = gtk_drawing_area_new();
	gtk_widget_set_events(
	    frame, GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);

	m_widgets = WidgetCanvasInternal::create();
	gtk_widget_set_events(GTK_WIDGET(m_widgets),
	                      GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK
	                          | GDK_BUTTON_RELEASE_MASK);
	g_signal_connect(
	    GTK_WIDGET(m_widgets), "motion-notify-event", G_CALLBACK(widgets_mouse_move), this);

	m_widgets->insertFullsize(frame);
	gtk_overlay_add_overlay(m_handle, GTK_WIDGET(m_widgets));

	gtk_widget_set_size_request(GTK_WIDGET(m_root), 500, 300);
	g_signal_connect(frame, "button-press-event", G_CALLBACK(canvas_mouse_down), this);

	g_signal_connect(gtk_scrolled_window_get_hadjustment(m_root),
	                 "value-changed",
	                 G_CALLBACK(canvas_viewport_moved),
	                 this);
	g_signal_connect(gtk_scrolled_window_get_vadjustment(m_root),
	                 "value-changed",
	                 G_CALLBACK(canvas_viewport_moved),
	                 this);

	gtk_widget_set_events(widget, GDK_POINTER_MOTION_MASK);
	g_signal_connect(widget, "motion-notify-event", G_CALLBACK(canvas_mouse_move), this);

	cnt.add(m_root);
	m_moving = nullptr;
	r_eh     = nullptr;
	g_signal_connect(widget, "map", G_CALLBACK(on_realized), this);
}

Texpainter::Ui::WidgetCanvasDetail::Impl::~Impl()
{
	r_eh   = nullptr;
	m_impl = nullptr;
	gtk_widget_destroy(GTK_WIDGET(m_root));
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

Texpainter::vec2_t Texpainter::Ui::WidgetCanvasDetail::viewportOffset() const
{
	return m_impl->viewportOffset();
}

void Texpainter::Ui::WidgetCanvasDetail::updateCanvasSize() { m_impl->updateCanvasSize(); }