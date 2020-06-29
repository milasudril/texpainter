//@	{"targets":[{"name":"separator.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./separator.hpp"
#include "./container.hpp"

#include <gtk/gtk.h>

class Texpainter::Ui::Separator::Impl: private Separator
{
public:
	Impl(Container& cnt, bool vertical);
	~Impl();

private:
	GtkSeparator* m_handle;
};

Texpainter::Ui::Separator::Separator(Container& cnt, bool vertical) noexcept
{
	m_impl = new Impl(cnt, vertical);
}

Texpainter::Ui::Separator::~Separator()
{
	delete m_impl;
}

Texpainter::Ui::Separator::Impl::Impl(Container& cnt, bool vertical): Separator(*this)
{
	auto widget = gtk_separator_new(vertical ? GTK_ORIENTATION_VERTICAL : GTK_ORIENTATION_HORIZONTAL);

	m_handle = GTK_SEPARATOR(widget);
	if(vertical)
	{
		gtk_widget_set_margin_start(widget, 2);
		gtk_widget_set_margin_end(widget, 2);
	}
	else
	{
		gtk_widget_set_margin_top(widget, 2);
		gtk_widget_set_margin_bottom(widget, 2);
	}
	g_object_ref_sink(m_handle);
	cnt.add(widget);
}

Texpainter::Ui::Separator::Impl::~Impl()
{
	m_impl = nullptr;
	gtk_widget_destroy(GTK_WIDGET(m_handle));
	g_object_unref(m_handle);
}
