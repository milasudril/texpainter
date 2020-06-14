//@	{
//@	 "targets":[{"name":"filler.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]
//@	}

#include "./filler.hpp"

#include <gtk/gtk.h>

class Texpainter::Ui::Filler::Impl: public Filler
{
public:
	Impl(Container& cnt);
	~Impl();

private:
	GtkDrawingArea* m_handle;
};

Texpainter::Ui::Filler::Filler(Container& cnt)
{
	m_impl = new Impl(cnt);
}

Texpainter::Ui::Filler::~Filler()
{
	delete m_impl;
}

Texpainter::Ui::Filler::Impl::Impl(Container& cnt): Filler(*this)
{
	auto widget = gtk_drawing_area_new();
	m_handle = GTK_DRAWING_AREA(widget);
	g_object_ref_sink(widget);
	cnt.add(widget);
}

Texpainter::Ui::Filler::Impl::~Impl()
{
	m_impl = nullptr;
	gtk_widget_destroy(GTK_WIDGET(m_handle));
	g_object_unref(m_handle);
}