//@	{"targets":[{"name":"listbox.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./listbox.hpp"

#include <gtk/gtk.h>

#include <vector>
#include <map>


class Texpainter::Ui::Listbox::Impl: private Listbox
{
public:
	Impl(Container& cnt);
	~Impl();

	void append(const char* option)
	{
		auto label = GTK_LABEL(gtk_label_new(option));
		gtk_label_set_justify(label, GTK_JUSTIFY_LEFT);
		gtk_widget_set_halign(GTK_WIDGET(label), GTK_ALIGN_START);

		auto row = GTK_LIST_BOX_ROW(gtk_list_box_row_new());
		gtk_container_add(GTK_CONTAINER(row), GTK_WIDGET(label));
		gtk_list_box_insert(m_handle, GTK_WIDGET(row), -1);

		m_row_index[row] = static_cast<int>(m_index_row.size());
		m_index_row.push_back(row);
	}

	void selected(int index) noexcept
	{
		auto eh = r_eh;
		r_eh    = nullptr;
		gtk_list_box_select_row(m_handle, m_index_row[index]);
		r_eh = eh;
	}

	int selected() const noexcept
	{
		auto list = gtk_list_box_get_selected_rows(m_handle);
		auto row  = GTK_LIST_BOX_ROW(list->data);
		g_list_free(list);

		return m_row_index.find(row)->second;
	}

	void eventHandler(void* event_handler, EventHandlerFunc func)
	{
		r_eh   = event_handler;
		r_func = func;
	}

	void scrollIntoView(int index)
	{
		auto row = m_index_row[index];
		int x{};
		int y{};
		gtk_widget_translate_coordinates(GTK_WIDGET(row), GTK_WIDGET(m_handle), 0, 0, &x, &y);
		auto adjustment = gtk_list_box_get_adjustment(m_handle);
		gtk_adjustment_set_value(
		    adjustment, static_cast<double>(y) - 0.33 * gtk_adjustment_get_page_size(adjustment));
	}

	char const* get(int index) const
	{
		return gtk_label_get_text(GTK_LABEL(gtk_bin_get_child(GTK_BIN(m_index_row[index]))));
	}

private:
	void* r_eh;
	EventHandlerFunc r_func;
	GtkScrolledWindow* m_root;
	GtkListBox* m_handle;

	std::vector<GtkListBoxRow*> m_index_row;
	std::map<GtkListBoxRow*, int> m_row_index;

	static void activated(GtkListBox*, GtkListBoxRow* row, gpointer user_data)
	{
		auto self = reinterpret_cast<Impl*>(user_data);
		if(self->r_eh != nullptr)
		{ self->r_func(self->r_eh, *self, self->m_row_index.find(row)->second); }
	}
};

Texpainter::Ui::Listbox::Listbox(Container& cnt) { m_impl = new Impl(cnt); }

Texpainter::Ui::Listbox::~Listbox() { delete m_impl; }

Texpainter::Ui::Listbox& Texpainter::Ui::Listbox::append(const char* option)
{
	m_impl->append(option);
	return *this;
}

Texpainter::Ui::Listbox& Texpainter::Ui::Listbox::selected(int index) noexcept
{
	m_impl->selected(index);
	return *this;
}

int Texpainter::Ui::Listbox::selected() const noexcept { return m_impl->selected(); }

Texpainter::Ui::Listbox& Texpainter::Ui::Listbox::eventHandler(void* event_handler,
                                                               EventHandlerFunc f)
{
	m_impl->eventHandler(event_handler, f);
	return *this;
}


Texpainter::Ui::Listbox::Impl::Impl(Container& cnt): Listbox(*this), r_eh{nullptr}
{
	auto scrolled_window = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(nullptr, nullptr));
	auto widget          = gtk_list_box_new();
	m_handle             = GTK_LIST_BOX(widget);
	gtk_list_box_set_activate_on_single_click(m_handle, FALSE);
	g_signal_connect(m_handle, "row-activated", G_CALLBACK(activated), this);
	gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(m_handle));
	gtk_widget_set_size_request(GTK_WIDGET(scrolled_window), 128, 128);
	m_root = scrolled_window;
	cnt.add(scrolled_window);
}

Texpainter::Ui::Listbox::Impl::~Impl()
{
	m_impl = nullptr;
	r_eh   = nullptr;
	gtk_widget_destroy(GTK_WIDGET(m_root));
}

Texpainter::Ui::Listbox& Texpainter::Ui::Listbox::scrollIntoView(int row) noexcept
{
	m_impl->scrollIntoView(row);
	return *this;
}

char const* Texpainter::Ui::Listbox::get(int index) const noexcept { return m_impl->get(index); }