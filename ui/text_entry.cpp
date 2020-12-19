//@	{"targets":[{"name":"text_entry.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./text_entry.hpp"

#include <gtk/gtk.h>

namespace
{
	static GtkCssProvider* s_smallstyle = nullptr;
	static size_t s_style_refcount      = 0;
}

class Texpainter::Ui::TextEntry::Impl: private TextEntry
{
public:
	Impl(Container& cnt);
	~Impl();

	void eventHandler(void* event_handler, EventHandlerFunc func)
	{
		r_eh   = event_handler;
		r_func = func;
	}

	const char* content() const noexcept { return gtk_entry_get_text(m_handle); }

	void content(const char* text) noexcept { return gtk_entry_set_text(m_handle, text); }

	void width(int n) noexcept
	{
		gtk_entry_set_width_chars(m_handle, n);
		gtk_entry_set_max_width_chars(m_handle, n);
	}

	void small(bool status)
	{
		auto context = gtk_widget_get_style_context(GTK_WIDGET(m_handle));
		if(status)
		{
			gtk_style_context_add_provider(context,
			                               GTK_STYLE_PROVIDER(s_smallstyle),
			                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION + 50);
		}
		else
		{
			gtk_style_context_remove_provider(context, GTK_STYLE_PROVIDER(s_smallstyle));
		}
	}

	void alignment(float x) { gtk_entry_set_alignment(m_handle, x); }

	void focus() { gtk_widget_grab_focus(GTK_WIDGET(m_handle)); }

	void enabled(bool status)
	{
		gtk_widget_set_sensitive(GTK_WIDGET(m_handle), status ? TRUE : FALSE);
	}

	void continuousUpdate(bool status) { m_continues_update = status; }

private:
	void* r_eh;
	bool m_continues_update;
	EventHandlerFunc r_func;
	GtkEntry* m_handle;

	static gboolean focus_callback(GtkWidget*, GdkEvent*, gpointer data)
	{
		auto self = reinterpret_cast<Impl*>(data);
		if(self->r_eh != nullptr)
		{
			if(self->r_eh != nullptr) { self->r_func(self->r_eh, *self); }
		}
		return FALSE;
	}

	static gboolean key_release(GtkWidget*, GdkEvent*, void* data)
	{
		auto self = reinterpret_cast<Impl*>(data);
		if(self->r_eh != nullptr && self->m_continues_update)
		{
			if(self->r_eh != nullptr) { self->r_func(self->r_eh, *self); }
		}
		return FALSE;
	}
};

Texpainter::Ui::TextEntry::TextEntry(Container& cnt) { m_impl = new Impl(cnt); }

Texpainter::Ui::TextEntry::~TextEntry() { delete m_impl; }

Texpainter::Ui::TextEntry& Texpainter::Ui::TextEntry::eventHandler(void* event_handler,
                                                                   EventHandlerFunc func)
{
	m_impl->eventHandler(event_handler, func);
	return *this;
}

const char* Texpainter::Ui::TextEntry::content() const noexcept { return m_impl->content(); }

Texpainter::Ui::TextEntry& Texpainter::Ui::TextEntry::content(const char* text)
{
	m_impl->content(text);
	return *this;
}

Texpainter::Ui::TextEntry& Texpainter::Ui::TextEntry::width(int n) noexcept
{
	m_impl->width(n);
	return *this;
}

Texpainter::Ui::TextEntry& Texpainter::Ui::TextEntry::small(bool status) noexcept
{
	m_impl->small(status);
	return *this;
}

Texpainter::Ui::TextEntry& Texpainter::Ui::TextEntry::alignment(float x) noexcept
{
	m_impl->alignment(x);
	return *this;
}

Texpainter::Ui::TextEntry& Texpainter::Ui::TextEntry::focus()
{
	m_impl->focus();
	return *this;
}


Texpainter::Ui::TextEntry::Impl::Impl(Container& cnt)
    : TextEntry{*this}
    , r_eh{nullptr}
    , m_continues_update{false}
{
	auto widget = gtk_entry_new();
	g_signal_connect(widget, "focus-out-event", G_CALLBACK(focus_callback), this);
	g_signal_connect(widget, "key-release-event", G_CALLBACK(key_release), this);
	m_handle = GTK_ENTRY(widget);

	if(s_style_refcount == 0)
	{
		s_smallstyle = gtk_css_provider_new();
		gtk_css_provider_load_from_data(
		    s_smallstyle, "*{font-size:0.8em;padding:1px;min-height:0px}", -1, nullptr);
	}
	++s_style_refcount;

	cnt.add(widget);
}

Texpainter::Ui::TextEntry::Impl::~Impl()
{
	m_impl = nullptr;
	r_eh   = nullptr;

	if(s_style_refcount != 0)
	{
		auto context = gtk_widget_get_style_context(GTK_WIDGET(m_handle));
		gtk_style_context_remove_provider(context, GTK_STYLE_PROVIDER(s_smallstyle));
	}
	else
	{
		--s_style_refcount;
		if(s_style_refcount == 0) { g_object_unref(s_smallstyle); }
	}

	gtk_widget_destroy(GTK_WIDGET(m_handle));
}

Texpainter::Ui::TextEntry& Texpainter::Ui::TextEntry::enabled(bool status) noexcept
{
	m_impl->enabled(status);
	return *this;
}

Texpainter::Ui::TextEntry& Texpainter::Ui::TextEntry::continuousUpdate(bool status) noexcept
{
	m_impl->continuousUpdate(status);
	return *this;
}
