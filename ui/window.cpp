//@	{
//@  "targets":[{"name":"window.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]
//@	}

#include "./window.hpp"

#include <gtk/gtk.h>
#include <string>

class Texpainter::Ui::Window::Impl final: private Window
{
public:
	Impl(char const* ti, Container* owner);
	~Impl();

	char const* title() const noexcept { return m_title.c_str(); }

	void title(char const* title_new)
	{
		gtk_window_set_title(m_handle, title_new);
		m_title = title_new;
	}

	void _add(GtkWidget* handle)
	{
		gtk_widget_set_margin_start(handle, 4);
		gtk_widget_set_margin_end(handle, 4);
		gtk_widget_set_margin_top(handle, 4);
		gtk_widget_set_margin_bottom(handle, 4);
		gtk_container_add(GTK_CONTAINER(m_handle), handle);
	}

	void _show()
	{
		gtk_widget_show_all(GTK_WIDGET(m_handle));
		gtk_window_present_with_time(m_handle, 0);
	}

	void _sensitive(bool val) { gtk_widget_set_sensitive(GTK_WIDGET(m_handle), val); }

	void* _toplevel() const { return m_handle; }

	void _killFocus() { gtk_window_set_focus(GTK_WINDOW(_toplevel()), NULL); }


	void eventHandler(void* eh, EventHandlerVtable const& vtable)
	{
		r_eh = eh;
		m_vt = vtable;
	}

	void modal(bool state) { gtk_window_set_modal(m_handle, state); }

	void defaultSize(Size2d size)
	{
		gtk_window_set_default_size(m_handle, size.width(), size.height());
	}

	void resize(Size2d size)
	{
		if(gtk_window_get_decorated(m_handle) == TRUE)
		{
			if(auto gdk_window = gtk_widget_get_window(GTK_WIDGET(m_handle)); gdk_window != nullptr)
			{
				GdkRectangle rect{};
				GtkAllocation alloc{};
				auto size_new = size;
				while(size_new == size)
				{
					gtk_main_iteration();
					gdk_window_get_frame_extents(gdk_window, &rect);
					gtk_widget_get_allocation(GTK_WIDGET(m_handle), &alloc);
					size = Size2d{size.width() - (rect.width - alloc.width),
					              size.height() - (rect.height - alloc.height)};
				}
				size_new = size;
			}
			else
			{
				return;
			}
		}
		gtk_window_resize(m_handle, size.width(), size.height());
	}

	void move(ScreenCoordinates coords)
	{
		gtk_window_move(m_handle, static_cast<int>(coords.x()), static_cast<int>(coords.y()));
	}

	void shrinkToContent() { gtk_window_resize(m_handle, 1, 1); }

	Size2d size() const
	{
		auto gdk_window = gtk_widget_get_window(GTK_WIDGET(m_handle));
		assert(gdk_window != nullptr);
		GdkRectangle rect{};
		gdk_window_get_frame_extents(gdk_window, &rect);
		return Size2d{static_cast<uint32_t>(rect.width), static_cast<uint32_t>(rect.height)};
	}

	ScreenCoordinates location() const
	{
		auto gdk_window = gtk_widget_get_window(GTK_WIDGET(m_handle));
		assert(gdk_window != nullptr);
		GdkRectangle rect{};
		gdk_window_get_frame_extents(gdk_window, &rect);
		return ScreenCoordinates{static_cast<double>(rect.x), static_cast<double>(rect.y)};
	}

	bool minimized() const { return m_minimized; }

	void minimized(bool status)
	{
		if(status) { gtk_window_iconify(m_handle); }
		else
		{
			gtk_window_deiconify(m_handle);
		}
	}

	bool maximized() const { return m_maximized; }

	void maximized(bool status)
	{
		if(status) { gtk_window_maximize(m_handle); }
		else
		{
			gtk_window_unmaximize(m_handle);
		}
	}


private:
	static gboolean delete_event(GtkWidget* widget, GdkEvent* event, void* user_data);
	static gboolean key_press(GtkWidget* widget, GdkEvent* event, void* user_data);
	static gboolean key_release(GtkWidget* widget, GdkEvent* event, void* user_data);
	static gboolean state_event(GtkWidget*, GdkEvent* e, gpointer user_data)
	{
		auto event        = reinterpret_cast<GdkEventWindowState const*>(e);
		auto self         = reinterpret_cast<Impl*>(user_data);
		self->m_minimized = (event->new_window_state == GDK_WINDOW_STATE_ICONIFIED);
		self->m_maximized = (event->new_window_state == GDK_WINDOW_STATE_MAXIMIZED);
		return TRUE;
	}

	void* r_eh;
	EventHandlerVtable m_vt;
	GtkWindow* m_handle;
	GtkWidget* r_focus_old;
	std::string m_title;
	GdkPixbuf* m_icon;

	bool m_minimized;
	bool m_maximized;
};

Texpainter::Ui::Window::Window(char const* title, Container* owner)
{
	m_impl = new Impl(title, owner);
}

Texpainter::Ui::Window::~Window() { delete m_impl; }

char const* Texpainter::Ui::Window::title() const noexcept { return m_impl->title(); }

Texpainter::Ui::Window& Texpainter::Ui::Window::title(char const* title_new)
{
	m_impl->title(title_new);
	return *this;
}

Texpainter::Ui::Window& Texpainter::Ui::Window::add(void* handle)
{
	m_impl->_add(GTK_WIDGET(handle));
	return *this;
}

Texpainter::Ui::Window& Texpainter::Ui::Window::show()
{
	m_impl->_show();
	return *this;
}

Texpainter::Ui::Window& Texpainter::Ui::Window::sensitive(bool val)
{
	m_impl->_sensitive(val);
	return *this;
}

Texpainter::Ui::Window& Texpainter::Ui::Window::killFocus()
{
	m_impl->_killFocus();
	return *this;
}

void* Texpainter::Ui::Window::toplevel() const { return m_impl->_toplevel(); }

Texpainter::Ui::Window& Texpainter::Ui::Window::eventHandler(void* event_handler,
                                                             EventHandlerVtable const& vt)
{
	m_impl->eventHandler(event_handler, vt);
	return *this;
}

Texpainter::Ui::Window& Texpainter::Ui::Window::modal(bool state)
{
	m_impl->modal(state);
	return *this;
}

Texpainter::Ui::Window::Impl::Impl(char const* ti, Container* owner)
    : Window{*this}
    , r_eh{nullptr}
    , r_focus_old{nullptr}
    , m_minimized{false}
    , m_maximized{false}
{
	auto widget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(widget, "delete-event", G_CALLBACK(delete_event), this);
	g_signal_connect(widget, "key-press-event", G_CALLBACK(key_press), this);
	g_signal_connect(widget, "key-release-event", G_CALLBACK(key_release), this);
	g_signal_connect(widget, "window-state-event", G_CALLBACK(state_event), this);

	m_handle = GTK_WINDOW(widget);
	title(ti);
	if(owner != nullptr) { gtk_window_set_transient_for(m_handle, GTK_WINDOW(owner->toplevel())); }
	m_icon = nullptr;
}

Texpainter::Ui::Window::Impl::~Impl()
{
	m_impl = nullptr;
	r_eh   = nullptr;
	gtk_widget_destroy(GTK_WIDGET(m_handle));
	if(m_icon != nullptr) { g_object_unref(m_icon); }
}

gboolean Texpainter::Ui::Window::Impl::delete_event(GtkWidget*, GdkEvent*, void* user_data)
{
	auto self = reinterpret_cast<Impl*>(user_data);
	if(self->r_eh != nullptr) { self->m_vt.on_close(self->r_eh, *self); }
	return TRUE;
}

gboolean Texpainter::Ui::Window::Impl::key_press(GtkWidget* widget,
                                                 GdkEvent* event,
                                                 void* user_data)
{
	auto self     = reinterpret_cast<Impl*>(user_data);
	auto& key     = event->key;
	auto scancode = Scancode{static_cast<uint8_t>(key.hardware_keycode - 8)};
#ifndef __linux__
	#waring "Scancode key offset is not tested. Pressing esc should print 1"
	printf("%d\n", key.hardware_keycode - 8);
#endif
	auto w = gtk_window_get_focus(GTK_WINDOW(widget));
	if(w != NULL)
	{
		self->r_focus_old = w;
		switch(scancode.value())
		{
			case Scancodes::Escape.value():
				gtk_window_set_focus(GTK_WINDOW(widget), NULL);
				if(self->r_eh != nullptr) { self->m_vt.on_key_down(self->r_eh, *self, scancode); }
				return TRUE;

			case Scancodes::Return.value():
			case Scancodes::ReturnNumpad.value():
				if(gtk_window_get_transient_for(GTK_WINDOW(widget)) != NULL)  // Dialog box
				{
					gtk_window_set_focus(GTK_WINDOW(widget), NULL);
					if(self->r_eh != nullptr)
					{ self->m_vt.on_key_down(self->r_eh, *self, scancode); }
				}
				return TRUE;
			default: return FALSE;
		}
	}
	if(scancode == Scancode{15} && self->r_focus_old != nullptr)  // Tab
	{
		gtk_window_set_focus(GTK_WINDOW(widget), self->r_focus_old);
		return TRUE;
	}
	if(self->r_eh != nullptr)
	{
		self->m_vt.on_key_down(self->r_eh, *self, scancode);
		return TRUE;
	}
	return FALSE;
}

gboolean Texpainter::Ui::Window::Impl::key_release(GtkWidget* widget,
                                                   GdkEvent* event,
                                                   void* user_data)
{
	auto w = gtk_window_get_focus(GTK_WINDOW(widget));
	if(w != NULL) { return FALSE; }
	auto self     = reinterpret_cast<Impl*>(user_data);
	auto& key     = event->key;
	auto scancode = Scancode{static_cast<uint8_t>(key.hardware_keycode - 8)};
#ifndef __linux__
	#waring "Scancode key offset is not tested. Pressing esc should print 1"
	printf("%d\n", key.hardware_keycode - 8);
#endif
	if(self->r_eh != nullptr) { self->m_vt.on_key_up(self->r_eh, *self, scancode); }
	return TRUE;
}

Texpainter::Ui::Window& Texpainter::Ui::Window::defaultSize(Size2d size)
{
	m_impl->defaultSize(size);
	return *this;
}


Texpainter::Ui::Window& Texpainter::Ui::Window::resize(Size2d size)
{
	m_impl->resize(size);
	return *this;
}

void Texpainter::Ui::Window::terminateApp() { gtk_main_quit(); }

Texpainter::Ui::Window& Texpainter::Ui::Window::move(ScreenCoordinates coords)
{
	m_impl->move(coords);
	return *this;
}

Texpainter::Ui::Window& Texpainter::Ui::Window::shrinkToContent()
{
	m_impl->shrinkToContent();
	return *this;
}


Texpainter::Size2d Texpainter::Ui::Window::size() const { return m_impl->size(); }

Texpainter::Ui::ScreenCoordinates Texpainter::Ui::Window::location() const
{
	return m_impl->location();
}

bool Texpainter::Ui::Window::minimized() const { return m_impl->minimized(); }

Texpainter::Ui::Window& Texpainter::Ui::Window::minimized(bool status)
{
	m_impl->minimized(status);
	return *this;
}

bool Texpainter::Ui::Window::maximized() const { return m_impl->maximized(); }


Texpainter::Ui::Window& Texpainter::Ui::Window::maximized(bool status)
{
	m_impl->maximized(status);
	return *this;
}