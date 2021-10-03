//@	{
//@  "targets":[{"name":"widget_multiplexer.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]
//@	}

#include "./widget_multiplexer.hpp"

#include <gtk/gtk.h>

#include <string>

class Texpainter::Ui::WidgetMultiplexer::Impl final: private WidgetMultiplexer
{
public:
	Impl(Container& cnt);
	~Impl();

	void _add(GtkWidget* handle) noexcept
	{
		gtk_stack_add_named(m_handle, handle, m_widget_name.c_str());
	}

	void _show() noexcept { gtk_widget_show_all(GTK_WIDGET(m_handle)); }

	void _sensitive(bool val) { gtk_widget_set_sensitive(GTK_WIDGET(m_handle), val); }

	void* _toplevel() const { return gtk_widget_get_toplevel(GTK_WIDGET(m_handle)); }

	void _killFocus() { gtk_window_set_focus(GTK_WINDOW(_toplevel()), NULL); }

	void widgetName(char const* widget_name) { m_widget_name = widget_name; }

	void showWidget(char const* widget_name)
	{
		gtk_stack_set_visible_child_name(m_handle, widget_name);
	}

private:
	static void destroy_callback(GtkWidget* object, gpointer user_data);
	GtkStack* m_handle;
	std::string m_widget_name;
};

Texpainter::Ui::WidgetMultiplexer::WidgetMultiplexer(Container& cnt)
{
	m_impl = new Texpainter::Ui::WidgetMultiplexer::Impl(cnt);
}

Texpainter::Ui::WidgetMultiplexer::~WidgetMultiplexer() { delete m_impl; }

Texpainter::Ui::WidgetMultiplexer& Texpainter::Ui::WidgetMultiplexer::add(void* handle)
{
	m_impl->_add(GTK_WIDGET(handle));
	return *this;
}

Texpainter::Ui::WidgetMultiplexer& Texpainter::Ui::WidgetMultiplexer::widgetName(
    char const* widget_name)
{
	m_impl->widgetName(widget_name);
	return *this;
}

Texpainter::Ui::WidgetMultiplexer& Texpainter::Ui::WidgetMultiplexer::show()
{
	m_impl->_show();
	return *this;
}

Texpainter::Ui::WidgetMultiplexer& Texpainter::Ui::WidgetMultiplexer::sensitive(bool val)
{
	m_impl->_sensitive(val);
	return *this;
}

Texpainter::Ui::WidgetMultiplexer& Texpainter::Ui::WidgetMultiplexer::killFocus()
{
	m_impl->_killFocus();
	return *this;
}

Texpainter::Ui::WidgetMultiplexer& Texpainter::Ui::WidgetMultiplexer::showWidget(
    char const* widget_name)
{
	m_impl->showWidget(widget_name);
	return *this;
}

void* Texpainter::Ui::WidgetMultiplexer::toplevel() const { return m_impl->_toplevel(); }

Texpainter::Ui::WidgetMultiplexer::Impl::Impl(Container& cnt): WidgetMultiplexer(*this)
{
	auto widget = gtk_stack_new();
	g_object_ref_sink(widget);
	cnt.add(widget);
	m_handle = GTK_STACK(widget);
}

Texpainter::Ui::WidgetMultiplexer::Impl::~Impl()
{
	m_impl = nullptr;
	gtk_widget_destroy(GTK_WIDGET(m_handle));
	g_object_unref(m_handle);
}