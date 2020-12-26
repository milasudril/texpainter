//@	{"targets":[{"name":"menu_item.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./menu_item.hpp"

#include <gtk/gtk.h>

class Texpainter::Ui::MenuItem::Impl: private MenuItem
{
public:
	Impl(Container& cnt, char const* label);
	~Impl();

	void eventHandler(void* event_handler, EventHandlerFunc func)
	{
		r_eh   = event_handler;
		r_func = func;
	}

	void label(char const* label) { gtk_menu_item_set_label(m_handle, label); }

	char const* label() const { return gtk_menu_item_get_label(m_handle); }

	void _add(GtkWidget* handle) noexcept { gtk_menu_item_set_submenu(m_handle, handle); }

	void _show() noexcept { gtk_widget_show_all(GTK_WIDGET(m_handle)); }

	void _sensitive(bool val) { gtk_widget_set_sensitive(GTK_WIDGET(m_handle), val); }

	void* _toplevel() const { return gtk_widget_get_toplevel(GTK_WIDGET(m_handle)); }

private:
	void* r_eh;
	EventHandlerFunc r_func;
	GtkMenuItem* m_handle;

	static void activate(GtkMenuItem*, void* data)
	{
		auto& self = *reinterpret_cast<Impl*>(data);
		if(self.r_eh != nullptr) { self.r_func(self.r_eh, self); }
	}
};

Texpainter::Ui::MenuItem::MenuItem(Container& cnt, char const* label)
{
	m_impl = new Impl(cnt, label);
}

Texpainter::Ui::MenuItem::~MenuItem() { delete m_impl; }

Texpainter::Ui::MenuItem::Impl::Impl(Container& cnt, char const* label): MenuItem{*this}
{
	r_eh        = nullptr;
	auto widget = gtk_menu_item_new_with_label(label);
	g_signal_connect(widget, "activate", G_CALLBACK(activate), this);
	m_handle = GTK_MENU_ITEM(widget);
	cnt.add(widget);
}

Texpainter::Ui::MenuItem::Impl::~Impl()
{
	m_impl = nullptr;
	gtk_widget_destroy(GTK_WIDGET(m_handle));
}

Texpainter::Ui::MenuItem& Texpainter::Ui::MenuItem::label(char const* label)
{
	m_impl->label(label);
	return *this;
}

Texpainter::Ui::MenuItem& Texpainter::Ui::MenuItem::eventHandler(void* event_handler,
                                                                 EventHandlerFunc f)
{
	m_impl->eventHandler(event_handler, f);
	return *this;
}


Texpainter::Ui::MenuItem& Texpainter::Ui::MenuItem::add(void* handle)
{
	m_impl->_add(GTK_WIDGET(handle));
	return *this;
}

Texpainter::Ui::MenuItem& Texpainter::Ui::MenuItem::show()
{
	m_impl->_show();
	return *this;
}

Texpainter::Ui::MenuItem& Texpainter::Ui::MenuItem::sensitive(bool val)
{
	m_impl->_sensitive(val);
	return *this;
}

void* Texpainter::Ui::MenuItem::toplevel() const { return m_impl->_toplevel(); }

char const* Texpainter::Ui::MenuItem::label() const { return m_impl->label(); }
