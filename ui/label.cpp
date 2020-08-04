//@	{"targets":[{"name":"label.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./label.hpp"

#include <gtk/gtk.h>

#include <algorithm>

namespace
{
	GtkCssProvider* s_smallstyle = nullptr;
	size_t s_style_refcount      = 0;
}

//GtkWidget *somewidget;
//gint wx, wy;
//gtk_widget_translate_coordinates(somewidget, gtk_widget_get_toplevel(somewidget), 0, 0, &wx, &wy);


class Texpainter::Ui::Label::Impl: private Label
{
public:
	Impl(Container& cnt, const char* text);
	~Impl();

	const char* content() const noexcept { return pango_layout_get_text(m_content); }

	void content(const char* text)
	{
		pango_layout_set_text(m_content, text, -1);
		//	Turn off wordwrap to get the maximum width of the text
		pango_layout_set_width(m_content, -1);
		pango_layout_get_pixel_extents(m_content, NULL, &m_content_rect);

		auto w = m_content_rect.width;
		if(!m_oneline)
		{
			//	Change with so the rendered text is at most 500 px
			auto w = std::min(m_content_rect.width, 500);
			pango_layout_set_wrap(m_content, PANGO_WRAP_WORD);
			pango_layout_set_width(m_content, PANGO_SCALE * w);
			pango_layout_get_pixel_extents(m_content, NULL, &m_content_rect);
		}

		//	Request size based on the size of the layout
		gtk_widget_set_size_request(GTK_WIDGET(m_handle), w, m_content_rect.height);
		gtk_widget_queue_draw(GTK_WIDGET(m_handle));
	}

	void small(bool status)
	{
		auto context = gtk_widget_get_style_context(GTK_WIDGET(m_handle));
		if(status)
		{
			gtk_style_context_add_provider(
			    context, GTK_STYLE_PROVIDER(s_smallstyle), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
		}
		else
		{
			gtk_style_context_remove_provider(context, GTK_STYLE_PROVIDER(s_smallstyle));
		}
		pango_layout_get_pixel_extents(m_content, NULL, &m_content_rect);
		gtk_widget_queue_draw(GTK_WIDGET(m_handle));
	}


	void alignment(float x)
	{
		m_alignment_x = x;
		if(x < 1.0f / 3.0f) { pango_layout_set_alignment(m_content, PANGO_ALIGN_LEFT); }
		else if(x >= 2.0f / 3.0f)
		{
			pango_layout_set_alignment(m_content, PANGO_ALIGN_RIGHT);
		}
		else
		{
			pango_layout_set_alignment(m_content, PANGO_ALIGN_CENTER);
		}

		gtk_widget_queue_draw(GTK_WIDGET(m_handle));
	}

	void oneline(bool status) { m_oneline = status; }

private:
	double m_alignment_x;
	GtkDrawingArea* m_handle;
	PangoLayout* m_content;
	PangoRectangle m_content_rect;
	bool m_oneline;
	static gboolean draw(GtkWidget* object, cairo_t* cr, void* obj);
	static void size_changed(GtkWidget* widget, GtkAllocation* allocation, void* obj);
};

Texpainter::Ui::Label::Label(Container& cnt, const char* text) { m_impl = new Impl(cnt, text); }

Texpainter::Ui::Label::~Label() { delete m_impl; }

const char* Texpainter::Ui::Label::content() const noexcept { return m_impl->content(); }

Texpainter::Ui::Label& Texpainter::Ui::Label::content(const char* x)
{
	m_impl->content(x);
	return *this;
}

Texpainter::Ui::Label& Texpainter::Ui::Label::small(bool status)
{
	m_impl->small(status);
	return *this;
}

Texpainter::Ui::Label& Texpainter::Ui::Label::alignment(float x)
{
	m_impl->alignment(x);
	return *this;
}


Texpainter::Ui::Label::Impl::Impl(Container& cnt, const char* text): Label{*this}
{
	m_oneline   = false;
	auto widget = gtk_drawing_area_new();

	m_handle = GTK_DRAWING_AREA(widget);
	g_object_ref_sink(m_handle);
	cnt.add(widget);

	if(s_style_refcount == 0)
	{
		s_smallstyle = gtk_css_provider_new();
		gtk_css_provider_load_from_data(s_smallstyle, "*{font-size:0.8em;padding:1px}", -1, NULL);
	}
	++s_style_refcount;

	gtk_widget_set_events(GTK_WIDGET(widget),
	                      GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK
	                          | GDK_BUTTON_RELEASE_MASK);
	g_signal_connect(m_handle, "draw", G_CALLBACK(draw), this);
	g_signal_connect(widget, "size-allocate", G_CALLBACK(size_changed), this);


	m_content = gtk_widget_create_pango_layout(widget, text);
	alignment(0.0f);
	pango_layout_set_wrap(m_content, PANGO_WRAP_WORD);  //	We can always word wrap now...
	pango_layout_get_pixel_extents(m_content, NULL, &m_content_rect);

	//	Change with so the rendered text is at most 500 px
	auto w = std::min(m_content_rect.width, 500);
	pango_layout_set_width(m_content, PANGO_SCALE * w);
	pango_layout_get_pixel_extents(m_content, NULL, &m_content_rect);

	//	Request size based on the size of the layout
	gtk_widget_set_size_request(widget, w, m_content_rect.height);
}

Texpainter::Ui::Label::Impl::~Impl()
{
	m_impl = nullptr;
	g_object_unref(m_content);
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
	g_object_unref(m_handle);
}

void Texpainter::Ui::Label::Impl::size_changed(GtkWidget* widget,
                                               GdkRectangle* allocation,
                                               void* obj)
{
	auto self = reinterpret_cast<Impl*>(obj);
	auto w    = gtk_widget_get_allocated_width(widget);

	if(!self->m_oneline) { pango_layout_set_width(self->m_content, PANGO_SCALE * w); }
	pango_layout_get_pixel_extents(self->m_content, NULL, &self->m_content_rect);
}


gboolean Texpainter::Ui::Label::Impl::draw(GtkWidget* widget, cairo_t* cr, void* obj)
{
	auto context = gtk_widget_get_style_context(GTK_WIDGET(widget));
	auto self    = reinterpret_cast<Impl*>(obj);
	auto h       = static_cast<double>(gtk_widget_get_allocated_height(widget));
	auto w       = static_cast<double>(gtk_widget_get_allocated_width(widget));

	auto x = self->m_content_rect.x + self->m_alignment_x * (w - self->m_content_rect.width);
	auto y = -self->m_content_rect.y + 0.5 * (h - self->m_content_rect.height);
	gtk_render_layout(context, cr, x, y, self->m_content);
	return TRUE;
}

Texpainter::Ui::Label& Texpainter::Ui::Label::oneline(bool status)
{
	m_impl->oneline(status);
	return *this;
}