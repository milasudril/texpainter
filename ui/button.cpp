//@	{"targets":[{"name":"button.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./button.hpp"

#include <gtk/gtk.h>

class Texpainter::Ui::Button::Impl: private Texpainter::Ui::Button
{
public:
	Impl(Container& cnt, const char* label);
	~Impl();

	void eventHandler(void* event_handler, EventHandlerFunc func)
	{
		r_eh = event_handler;
		r_func = func;
	}

	const char* label() const noexcept
	{
		return gtk_button_get_label(GTK_BUTTON(m_handle));
	}

	void label(const char* text) noexcept
	{
		return gtk_button_set_label(GTK_BUTTON(m_handle), text);
	}

	void state(bool s) noexcept
	{
		auto eh = r_eh;
		r_eh = nullptr;
		gtk_toggle_button_set_active(m_handle, s);
		r_eh = eh;
	}

	bool state() const noexcept
	{
		return gtk_toggle_button_get_active(m_handle);
	}

	void focus() noexcept
	{
		gtk_widget_grab_focus(GTK_WIDGET(m_handle));
	}

private:
	void* r_eh;
	EventHandlerFunc r_func;
	GtkToggleButton* m_handle;

	static void clicked(GtkWidget* widget, gpointer data);
	static gboolean focus_in(GtkWidget* widget, GdkEvent* event, gpointer user_data);
};

Texpainter::Ui::Button::Button(Container& cnt, const char* label)
{
	m_impl = new Impl(cnt, label);
}

Texpainter::Ui::Button::~Button()
{
	delete m_impl;
}

Texpainter::Ui::Button& Texpainter::Ui::Button::eventHandler(void* event_handler,
                                                             EventHandlerFunc func)
{
	m_impl->eventHandler(event_handler, func);
	return *this;
}

const char* Texpainter::Ui::Button::label() const noexcept
{
	return m_impl->label();
}

Texpainter::Ui::Button& Texpainter::Ui::Button::label(const char* text)
{
	m_impl->label(text);
	return *this;
}

Texpainter::Ui::Button& Texpainter::Ui::Button::state(bool s) noexcept
{
	m_impl->state(s);
	return *this;
}

Texpainter::Ui::Button& Texpainter::Ui::Button::focus() noexcept
{
	m_impl->focus();
	return *this;
}


bool Texpainter::Ui::Button::state() const noexcept
{
	return m_impl->state();
}


Texpainter::Ui::Button::Impl::Impl(Container& cnt, const char* lab):
   Texpainter::Ui::Button{*this},
   r_eh{nullptr}
{
	auto widget = gtk_toggle_button_new();
	g_signal_connect(widget, "clicked", G_CALLBACK(clicked), this);
	m_handle = GTK_TOGGLE_BUTTON(widget);
	g_object_ref_sink(widget);
	cnt.add(widget);
	label(lab);
}

Texpainter::Ui::Button::Impl::~Impl()
{
	m_impl = nullptr;
	r_eh = nullptr;
	gtk_widget_destroy(GTK_WIDGET(m_handle));
	g_object_unref(m_handle);
}

void Texpainter::Ui::Button::Impl::clicked(GtkWidget* widget, gpointer data)
{
	auto self = reinterpret_cast<Impl*>(data);
	if(self->r_eh != nullptr) { self->r_func(self->r_eh, *self); }
}
