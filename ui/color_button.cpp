//@	{"targets":[{"name":"color_button.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./color_button.hpp"
#include "./container.hpp"

#include <gtk/gtk.h>

class Texpainter::Ui::ColorButton::Impl: private ColorButton
{
public:
	Impl(Container& cnt, PixelStore::Pixel color, double radius, vec2_t location);
	~Impl();

private:
	GtkDrawingArea* m_handle;
};

Texpainter::Ui::ColorButton::ColorButton(Container& cnt,
                                         PixelStore::Pixel color,
                                         double radius,
                                         vec2_t location)
{
	m_impl = new Impl(cnt, color, radius, location);
}

Texpainter::Ui::ColorButton::~ColorButton() { delete m_impl; }

Texpainter::Ui::ColorButton::Impl::Impl(Container& cnt,
                                        PixelStore::Pixel color,
                                        double radius,
                                        vec2_t location)
    : ColorButton{*this}
{
	auto widget = gtk_drawing_area_new();

	m_handle = GTK_DRAWING_AREA(widget);
	cnt.add(widget);
}

Texpainter::Ui::ColorButton::Impl::~Impl()
{
	m_impl = nullptr;
	gtk_widget_destroy(GTK_WIDGET(m_handle));
}
