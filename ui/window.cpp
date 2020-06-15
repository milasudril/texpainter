//@	{
//@  "targets":[{"name":"window.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]
//@	}

#include "ui/window.hpp"

#include <gtk/gtk.h>
#include <string>

class Texpainter::Ui::Window::Impl final: private Window
{
public:
	Impl(char const* ti, Container* owner);
	~Impl();

	char const* title() const noexcept
	{
		return m_title.c_str();
	}

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
	}

	void _sensitive(bool val)
	{
		gtk_widget_set_sensitive(GTK_WIDGET(m_handle), val);
	}

	void* _toplevel() const
	{
		return m_handle;
	}

	void eventHandler(void* eh, EventHandlerVtable const& vtable)
	{
		r_eh = eh;
		m_vt = vtable;
	}

	void modal(bool state)
	{
		gtk_window_set_modal(m_handle, state);
	}

	void defaultSize(Geom::Dimension dim)
	{
		gtk_window_set_default_size(m_handle, dim.width(), dim.height());
	}


private:
	static gboolean delete_event(GtkWidget* widget, GdkEvent* event, void* user_data);
	static gboolean key_press(GtkWidget* widget, GdkEvent* event, void* user_data);
	static gboolean key_release(GtkWidget* widget, GdkEvent* event, void* user_data);

	void* r_eh;
	EventHandlerVtable m_vt;
	GtkWindow* m_handle;
	GtkWidget* r_focus_old;
	std::string m_title;
	GdkPixbuf* m_icon;
};

Texpainter::Ui::Window::Window(char const* title, Container* owner)
{
	m_impl = new Impl(title, owner);
}

Texpainter::Ui::Window::~Window()
{
	delete m_impl;
}

char const* Texpainter::Ui::Window::title() const noexcept
{
	return m_impl->title();
}

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

void* Texpainter::Ui::Window::toplevel() const
{
	return m_impl->_toplevel();
}

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

Texpainter::Ui::Window::Impl::Impl(char const* ti, Container* owner):
   Window{*this},
   r_eh{nullptr},
   r_focus_old{nullptr}
{
	auto widget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(widget, "delete-event", G_CALLBACK(delete_event), this);
	g_signal_connect(widget, "key-press-event", G_CALLBACK(key_press), this);
	g_signal_connect(widget, "key-release-event", G_CALLBACK(key_release), this);
	m_handle = GTK_WINDOW(widget);
	title(ti);
	if(owner != nullptr) { gtk_window_set_transient_for(m_handle, GTK_WINDOW(owner->toplevel())); }
	m_icon = nullptr;
}

Texpainter::Ui::Window::Impl::~Impl()
{
	m_impl = nullptr;
	r_eh = nullptr;
	gtk_widget_destroy(GTK_WIDGET(m_handle));
	if(m_icon != nullptr) { g_object_unref(m_icon); }
}

gboolean
Texpainter::Ui::Window::Impl::delete_event(GtkWidget* widget, GdkEvent* event, void* user_data)
{
	auto self = reinterpret_cast<Impl*>(user_data);
	if(self->r_eh != nullptr) { self->m_vt.on_close(self->r_eh, *self); }
	return TRUE;
}

gboolean
Texpainter::Ui::Window::Impl::key_press(GtkWidget* widget, GdkEvent* event, void* user_data)
{
	auto self = reinterpret_cast<Impl*>(user_data);
	auto& key = event->key;
	auto scancode = key.hardware_keycode - 8;
#ifndef __linux__
#waring "Scancode key offset is not tested. Pressing esc should print 1"
	printf("%d\n", key.hardware_keycode - 8);
#endif
	auto w = gtk_window_get_focus(GTK_WINDOW(widget));
	if(w != NULL)
	{
		self->r_focus_old = w;
		switch(scancode)
		{
			case 1: //	ESC
				gtk_window_set_focus(GTK_WINDOW(widget), NULL);
				if(self->r_eh != nullptr) { self->m_vt.on_key_down(self->r_eh, *self, scancode); }
				break;
			case 28:                                                      //	RETURN
				if(gtk_window_get_transient_for(GTK_WINDOW(widget)) != NULL) //	Dialog box
				{
					gtk_window_set_focus(GTK_WINDOW(widget), NULL);
					if(self->r_eh != nullptr) { self->m_vt.on_key_down(self->r_eh, *self, scancode); }
				}
				break;
		}
		return FALSE;
	}
	if(scancode == 15 && self->r_focus_old != nullptr)
	{
		gtk_window_set_focus(GTK_WINDOW(widget), self->r_focus_old);
		return TRUE;
	}
	if(self->r_eh != nullptr) { self->m_vt.on_key_down(self->r_eh, *self, scancode); }
	return TRUE;
}

gboolean
Texpainter::Ui::Window::Impl::key_release(GtkWidget* widget, GdkEvent* event, void* user_data)
{
	auto w = gtk_window_get_focus(GTK_WINDOW(widget));
	if(w != NULL) { return FALSE; }
	auto self = reinterpret_cast<Impl*>(user_data);
	auto& key = event->key;
	auto scancode = key.hardware_keycode - 8;
#ifndef __linux__
#waring "Scancode key offset is not tested. Pressing esc should print 1"
	printf("%d\n", key.hardware_keycode - 8);
#endif
	if(self->r_eh != nullptr) { self->m_vt.on_key_up(self->r_eh, *self, scancode); }
	return TRUE;
}

Texpainter::Ui::Window& Texpainter::Ui::Window::defaultSize(Geom::Dimension dim)
{
	m_impl->defaultSize(dim);
	return *this;
}

void Texpainter::Ui::Window::terminateApp()
{
	gtk_main_quit();
}
