//@	{"targets":[{"name":"combobox.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./combobox.hpp"

#include <gtk/gtk.h>

class Texpainter::Ui::Combobox::Impl: private Combobox
{
public:
	Impl(Container& cnt);
	~Impl();

	void append(const char* option) { gtk_combo_box_text_append_text(m_handle, option); }

	void replace(int index, const char* option)
	{
		auto i  = selected();
		auto eh = r_eh;
		r_eh    = nullptr;
		gtk_combo_box_text_remove(m_handle, index);
		gtk_combo_box_text_insert(m_handle, index, NULL, option);
		gtk_combo_box_set_active(GTK_COMBO_BOX(m_handle), i);
		r_eh = eh;
	}

	void selected(int index) noexcept
	{
		auto eh = r_eh;
		r_eh    = nullptr;
		gtk_combo_box_set_active(GTK_COMBO_BOX(m_handle), index);
		r_eh = eh;
	}

	int selected() const noexcept { return gtk_combo_box_get_active(GTK_COMBO_BOX(m_handle)); }

	void clear() noexcept
	{
		auto eh = r_eh;
		r_eh    = nullptr;
		gtk_combo_box_text_remove_all(m_handle);
		r_eh = eh;
	}

	void eventHandler(void* event_handler, EventHandlerFunc func)
	{
		r_eh   = event_handler;
		r_func = func;
	}

private:
	void* r_eh;
	EventHandlerFunc r_func;
	GtkComboBoxText* m_handle;

	static void changed_callback(GtkComboBox* widget, void* listboxgtk)
	{
		auto self = reinterpret_cast<Impl*>(listboxgtk);
		if(self->r_eh != nullptr) { self->r_func(self->r_eh, *self); }
	}
};

Texpainter::Ui::Combobox::Combobox(Container& cnt) { m_impl = new Impl(cnt); }

Texpainter::Ui::Combobox::~Combobox() { delete m_impl; }

Texpainter::Ui::Combobox& Texpainter::Ui::Combobox::append(const char* option)
{
	m_impl->append(option);
	return *this;
}

Texpainter::Ui::Combobox& Texpainter::Ui::Combobox::replace(int index, const char* option)
{
	m_impl->replace(index, option);
	return *this;
}

Texpainter::Ui::Combobox& Texpainter::Ui::Combobox::selected(int index) noexcept
{
	m_impl->selected(index);
	return *this;
}

int Texpainter::Ui::Combobox::selected() const noexcept { return m_impl->selected(); }

Texpainter::Ui::Combobox& Texpainter::Ui::Combobox::clear() noexcept
{
	m_impl->clear();
	return *this;
}

Texpainter::Ui::Combobox& Texpainter::Ui::Combobox::eventHandler(void* event_handler,
                                                                 EventHandlerFunc f)
{
	m_impl->eventHandler(event_handler, f);
	return *this;
}


Texpainter::Ui::Combobox::Impl::Impl(Container& cnt): Combobox(*this), r_eh{nullptr}
{
	auto widget = gtk_combo_box_text_new();
	m_handle    = GTK_COMBO_BOX_TEXT(widget);
	g_signal_connect(widget, "changed", G_CALLBACK(changed_callback), this);
	cnt.add(widget);
}

Texpainter::Ui::Combobox::Impl::~Impl()
{
	m_impl = nullptr;
	r_eh   = nullptr;
	gtk_widget_destroy(GTK_WIDGET(m_handle));
}