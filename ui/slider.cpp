//@	{"targets":[{"name":"slider.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "slider.hpp"

#include <gtk/gtk.h>

class Texpainter::Ui::Slider::Impl: private Slider
{
public:
	Impl(Container& cnt, bool vertical);
	~Impl();

	void eventHandler(void* event_handler, EventHandlerFunc func)
	{
		r_eh = event_handler;
		r_func = func;
	}

	double value() const noexcept
	{
		return gtk_range_get_value(GTK_RANGE(m_handle));
	}

	void value(double x) noexcept
	{
		gtk_range_set_value(GTK_RANGE(m_handle), x);
	}

private:
	void* r_eh;
	EventHandlerFunc r_func;
	GtkScale* m_handle;
	static gboolean changed_callback(GtkWidget* widget, gpointer data);
};

Texpainter::Ui::Slider::Slider(Container& cnt, bool vertical)
{
	m_impl = new Impl(cnt, vertical);
}

Texpainter::Ui::Slider::~Slider()
{
	delete m_impl;
}

Texpainter::Ui::Slider& Texpainter::Ui::Slider::eventHandler(void* event_handler,
                                                             EventHandlerFunc f)
{
	m_impl->eventHandler(event_handler, f);
	return *this;
}

double Texpainter::Ui::Slider::value() const noexcept
{
	return m_impl->value();
}

Texpainter::Ui::Slider& Texpainter::Ui::Slider::value(double x)
{
	m_impl->value(x);
	return *this;
}

Texpainter::Ui::Slider::Impl::Impl(Container& cnt, bool vertical): Slider{*this}, r_eh(nullptr)
{
	auto widget = gtk_scale_new_with_range(
	   vertical ? GTK_ORIENTATION_VERTICAL : GTK_ORIENTATION_HORIZONTAL, 0, 1, 1e-3);
	gtk_range_set_inverted(GTK_RANGE(widget), vertical); //	GTK draws the slider upside down
	gtk_scale_set_draw_value(GTK_SCALE(widget), FALSE);
	gtk_widget_set_size_request(widget, vertical ? 32 : 96, vertical ? 96 : 32);

	g_signal_connect(widget, "value-changed", G_CALLBACK(changed_callback), this);
	m_handle = GTK_SCALE(widget);
	g_object_ref_sink(widget);
	cnt.add(widget);
}

Texpainter::Ui::Slider::Impl::~Impl()
{
	m_impl = nullptr;
	r_eh = nullptr;
	gtk_widget_destroy(GTK_WIDGET(m_handle));
	g_object_unref(m_handle);
}

gboolean Texpainter::Ui::Slider::Impl::changed_callback(GtkWidget* widget, gpointer data)
{
	auto state = reinterpret_cast<Impl*>(data);
	if(state->r_eh != nullptr) { state->r_func(state->r_eh, *state); }
	return FALSE;
}