//@	{"targets":[{"name":"menu_bar.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./menu_bar.hpp"

#include <gtk/gtk.h>

class Texpainter::Ui::MenuBar::Impl: private MenuBar
{
public:
	Impl(Container& cnt);
	~Impl();

	void _add(GtkWidget* handle) noexcept
	{
		gtk_container_add(GTK_CONTAINER(m_handle), handle);
	}

	void _show() noexcept
	{
		gtk_widget_show_all(GTK_WIDGET(m_handle));
	}

	void _sensitive(bool val)
	{
		gtk_widget_set_sensitive(GTK_WIDGET(m_handle), val);
	}

	void* _toplevel() const
	{
		return gtk_widget_get_toplevel(GTK_WIDGET(m_handle));
	}

private:
	GtkMenuBar* m_handle;
};

Texpainter::Ui::MenuBar::Impl::Impl(Container& cnt): MenuBar{*this}
{
	auto widget = gtk_menu_bar_new();
	m_handle = GTK_MENU_BAR(widget);
	cnt.add(widget);
}

Texpainter::Ui::MenuBar::Impl::~Impl()
{
	m_impl = nullptr;
	gtk_widget_destroy(GTK_WIDGET(m_handle));
}

Texpainter::Ui::MenuBar::MenuBar(Container& cnt)
{
	m_impl = new Impl(cnt);
}

Texpainter::Ui::MenuBar::~MenuBar()
{
	delete m_impl;
}

Texpainter::Ui::MenuBar& Texpainter::Ui::MenuBar::add(void* handle)
{
	m_impl->_add(GTK_WIDGET(handle));
	return *this;
}

Texpainter::Ui::MenuBar& Texpainter::Ui::MenuBar::show()
{
	m_impl->_show();
	return *this;
}

Texpainter::Ui::MenuBar& Texpainter::Ui::MenuBar::sensitive(bool val)
{
	m_impl->_sensitive(val);
	return *this;
}

void* Texpainter::Ui::MenuBar::toplevel() const
{
	return m_impl->_toplevel();
}
