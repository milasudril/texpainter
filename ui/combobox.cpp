//@	{"targets":[{"name":"combobox.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./combobox.hpp"

#include <gtk/gtk.h>

#include <map>

class Texpainter::Ui::Combobox::Impl: private Combobox
{
public:
	Impl(Container& cnt);
	~Impl();

	void append(const char* option)
	{
		gtk_combo_box_text_append_text(m_handle, option);
		m_index_to_string[m_current_index] = std::string{option};
		m_string_to_index[option]          = m_current_index;
		++m_current_index;
	}

	void replace(int index, const char* option)
	{
		auto i  = selectedInt();
		auto eh = r_eh;
		r_eh    = nullptr;

		gtk_combo_box_text_remove(m_handle, index);
		gtk_combo_box_text_insert(m_handle, index, NULL, option);
		gtk_combo_box_set_active(GTK_COMBO_BOX(m_handle), i);

		m_index_to_string[index]  = option;
		m_string_to_index[option] = index;
		m_string_to_index.erase(option);

		r_eh = eh;
	}

	void selected(int index) noexcept
	{
		auto eh = r_eh;
		r_eh    = nullptr;
		gtk_combo_box_set_active(GTK_COMBO_BOX(m_handle), index);
		r_eh = eh;
	}

	void selected(char const* str)
	{
		auto i = m_string_to_index.find(str);
		if(i == std::end(m_string_to_index)) [[unlikely]]
			{
				selected(-1);
				return;
			}
		selected(i->second);
	}

	int selectedInt() const noexcept { return gtk_combo_box_get_active(GTK_COMBO_BOX(m_handle)); }

	char const* selectedString() const noexcept
	{
		auto i = m_index_to_string.find(selectedInt());
		if(i == std::end(m_index_to_string)) { return nullptr; }
		return i->second.c_str();
	}


	void clear() noexcept
	{
		auto eh = r_eh;
		r_eh    = nullptr;
		gtk_combo_box_text_remove_all(m_handle);
		r_eh            = eh;
		m_current_index = 0;
		m_string_to_index.clear();
		m_index_to_string.clear();
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

	std::map<int, std::string> m_index_to_string;
	std::map<std::string, int, std::less<>> m_string_to_index;
	int m_current_index;

	static void changed_callback(GtkComboBox*, void* listboxgtk)
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


Texpainter::Ui::Combobox::Impl::Impl(Container& cnt)
    : Combobox(*this)
    , r_eh{nullptr}
    , m_current_index{0}
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

template<>
int Texpainter::Ui::Combobox::selected<int>() const noexcept
{
	return m_impl->selectedInt();
}

template<>
char const* Texpainter::Ui::Combobox::selected<char const*>() const noexcept
{
	return m_impl->selectedString();
}

Texpainter::Ui::Combobox& Texpainter::Ui::Combobox::selected(char const* str) noexcept
{
	m_impl->selected(str);
	return *this;
}

int Texpainter::Ui::Combobox::selected() const noexcept { return selected<int>(); }