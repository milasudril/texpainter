//@	{"targets":[{"name":"menu.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./menu.hpp"

#include <gtk/gtk.h>

class Texpainter::Ui::Menu::Impl: private Menu
{
public:
	Impl(Container& cnt);
	~Impl();

	void _add(GtkWidget* handle) noexcept
	{
		gtk_menu_attach(m_handle, handle, 0, 1, m_k, m_k + 1);
		++m_k;
	}

	void _show() noexcept { gtk_widget_show_all(GTK_WIDGET(m_handle)); }

	void _sensitive(bool val) { gtk_widget_set_sensitive(GTK_WIDGET(m_handle), val); }

	void* _toplevel() const { return gtk_widget_get_toplevel(GTK_WIDGET(m_handle)); }

private:
	GtkMenu* m_handle;
	size_t m_k;
};

Texpainter::Ui::Menu::Impl::Impl(Container& cnt): Menu{*this}, m_k{0}
{
	auto widget = gtk_menu_new();
	m_handle    = GTK_MENU(widget);
	cnt.add(widget);
}

Texpainter::Ui::Menu::Impl::~Impl()
{
	m_impl = nullptr;
	gtk_widget_destroy(GTK_WIDGET(m_handle));
}

Texpainter::Ui::Menu::Menu(Container& cnt) { m_impl = new Impl(cnt); }

Texpainter::Ui::Menu::~Menu() { delete m_impl; }

Texpainter::Ui::Menu& Texpainter::Ui::Menu::add(void* handle)
{
	m_impl->_add(GTK_WIDGET(handle));
	return *this;
}

Texpainter::Ui::Menu& Texpainter::Ui::Menu::show()
{
	m_impl->_show();
	return *this;
}

Texpainter::Ui::Menu& Texpainter::Ui::Menu::sensitive(bool val)
{
	m_impl->_sensitive(val);
	return *this;
}

void* Texpainter::Ui::Menu::toplevel() const { return m_impl->_toplevel(); }
