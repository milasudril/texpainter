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
		r_eh = event_handler;
		r_func = func;
	}

private:
	void* r_eh;
	EventHandlerFunc r_func;
	GtkMenuItem* m_handle;

	static void activate(GtkMenuItem*, void* data)
	{
		auto& self = *reinterpret_cast<Impl*>(data);
		if(self.r_eh != nullptr)
		{
			self.r_func(self.r_eh, self);
		}
	}
};

Texpainter::Ui::MenuItem::MenuItem(Container& cnt, char const* label)
{
	m_impl = new Impl(cnt, label);
}

Texpainter::Ui::MenuItem::~MenuItem()
{
	delete m_impl;
}

Texpainter::Ui::MenuItem::Impl::Impl(Container& cnt, char const* label): MenuItem{*this}
{
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

Texpainter::Ui::MenuItem& Texpainter::Ui::MenuItem::eventHandler(void* event_handler, EventHandlerFunc f)
{
	m_impl->eventHandler(event_handler, f);
	return *this;
}
