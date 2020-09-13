//@	{"targets":[{"name":"checkable_menu_item.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./checkable_menu_item.hpp"

#include <gtk/gtk.h>

class Texpainter::Ui::CheckableMenuItem::Impl: private CheckableMenuItem
{
public:
	Impl(Container& cnt, char const* label);
	~Impl();

	void eventHandler(void* event_handler, EventHandlerFunc func)
	{
		r_eh   = event_handler;
		r_func = func;
	}

	void label(char const* label) { gtk_menu_item_set_label(GTK_MENU_ITEM(m_handle), label); }

	void _add(GtkWidget* handle) noexcept
	{
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(m_handle), handle);
	}

	void _show() noexcept { gtk_widget_show_all(GTK_WIDGET(m_handle)); }

	void _sensitive(bool val) { gtk_widget_set_sensitive(GTK_WIDGET(m_handle), val); }

	void* _toplevel() const { return gtk_widget_get_toplevel(GTK_WIDGET(m_handle)); }

	void status(bool val)
	{
		auto eh = r_eh;
		r_eh    = nullptr;
		gtk_check_menu_item_set_active(m_handle, val);
		r_eh = eh;
	}

	bool status() const { return gtk_check_menu_item_get_active(m_handle); }

private:
	void* r_eh;
	EventHandlerFunc r_func;
	GtkCheckMenuItem* m_handle;

	static void activate(GtkCheckMenuItem*, void* data)
	{
		auto& self = *reinterpret_cast<Impl*>(data);
		if(self.r_eh != nullptr) { self.r_func(self.r_eh, self); }
	}
};

Texpainter::Ui::CheckableMenuItem::CheckableMenuItem(Container& cnt, char const* label)
{
	m_impl = new Impl(cnt, label);
}

Texpainter::Ui::CheckableMenuItem::~CheckableMenuItem() { delete m_impl; }

Texpainter::Ui::CheckableMenuItem::Impl::Impl(Container& cnt, char const* label)
    : CheckableMenuItem{*this}
{
	r_eh        = nullptr;
	auto widget = gtk_check_menu_item_new_with_label(label);
	g_signal_connect(widget, "activate", G_CALLBACK(activate), this);
	m_handle = GTK_CHECK_MENU_ITEM(widget);
	cnt.add(widget);
}

Texpainter::Ui::CheckableMenuItem::Impl::~Impl()
{
	m_impl = nullptr;
	gtk_widget_destroy(GTK_WIDGET(m_handle));
}

Texpainter::Ui::CheckableMenuItem& Texpainter::Ui::CheckableMenuItem::label(char const* label)
{
	m_impl->label(label);
	return *this;
}

Texpainter::Ui::CheckableMenuItem& Texpainter::Ui::CheckableMenuItem::status(bool val)
{
	m_impl->status(val);
	return *this;
}

bool Texpainter::Ui::CheckableMenuItem::status() const { return m_impl->status(); }

Texpainter::Ui::CheckableMenuItem& Texpainter::Ui::CheckableMenuItem::eventHandler(
    void* event_handler, EventHandlerFunc f)
{
	m_impl->eventHandler(event_handler, f);
	return *this;
}


Texpainter::Ui::CheckableMenuItem& Texpainter::Ui::CheckableMenuItem::add(void* handle)
{
	m_impl->_add(GTK_WIDGET(handle));
	return *this;
}

Texpainter::Ui::CheckableMenuItem& Texpainter::Ui::CheckableMenuItem::show()
{
	m_impl->_show();
	return *this;
}

Texpainter::Ui::CheckableMenuItem& Texpainter::Ui::CheckableMenuItem::sensitive(bool val)
{
	m_impl->_sensitive(val);
	return *this;
}

void* Texpainter::Ui::CheckableMenuItem::toplevel() const { return m_impl->_toplevel(); }