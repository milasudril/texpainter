//@	{"targets":[{"name":"palette_view.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./palette_view.hpp"
#include "model/image.hpp"
#include "geom/dimension.hpp"

#include <gtk/gtk.h>

#include <cassert>

namespace
{
	bool realloc_surface_needed(Texpainter::Model::Palette const& new_pal,
	                            Texpainter::Model::Palette const* old_pal)
	{
		if(old_pal == nullptr) { return true; }

		return std::size(new_pal) != std::size(*old_pal);
	}

	constexpr auto ItemSize = 32;
}

class Texpainter::Ui::PaletteView::Impl: private PaletteView
{
public:
	explicit Impl(Container& cnt): PaletteView{*this}
	{
		auto widget = gtk_drawing_area_new();
		g_object_ref_sink(widget);
		cnt.add(widget);
		m_handle = GTK_DRAWING_AREA(widget);
		r_pal = nullptr;
		r_eh = nullptr;
		gtk_widget_add_events(widget,
		                      GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK
		                         | GDK_KEY_PRESS_MASK | GDK_SCROLL_MASK);
		g_signal_connect(G_OBJECT(widget), "draw", G_CALLBACK(draw_callback), this);
		g_signal_connect(G_OBJECT(widget), "button-press-event", G_CALLBACK(on_mouse_down), this);
		g_signal_connect(G_OBJECT(widget), "button-release-event", G_CALLBACK(on_mouse_up), this);
		g_signal_connect(G_OBJECT(widget), "motion-notify-event", G_CALLBACK(on_mouse_move), this);
	}

	~Impl()
	{
		gtk_widget_destroy(GTK_WIDGET(m_handle));
		g_object_unref(m_handle);
		m_impl = nullptr;
	}

	void render(Geom::Dimension dim, cairo_t* cr)
	{
		auto context = gtk_widget_get_style_context(GTK_WIDGET(m_handle));
		gtk_render_background(context, cr, 0, 0, dim.width(), dim.height());
		if(r_pal != nullptr)
		{
			cairo_set_line_width(cr, 4);
			std::for_each(std::begin(*r_pal), std::end(*r_pal), [cr, k = 0](auto color) mutable {
				cairo_set_source_rgba(cr, color.red(), color.green(), color.blue(), color.alpha());
				cairo_rectangle(cr, k * ItemSize, 0, ItemSize, ItemSize);
				cairo_fill_preserve(cr);
				cairo_set_source_rgba(cr, 0.73, 0.73, 0.73, 1.0f);
				cairo_stroke(cr);
				++k;
			});
		}
	}

	void palette(Model::Palette const& pal)
	{
		if(realloc_surface_needed(pal, r_pal))
		{ gtk_widget_set_size_request(GTK_WIDGET(m_handle), ItemSize * std::size(pal), ItemSize); }
		r_pal = &pal;
		update();
	}

	void update()
	{
		gtk_widget_queue_draw(GTK_WIDGET(m_handle));
	}

	void eventHandler(void* event_handler, EventHandlerVtable const& vtable)
	{
		r_eh = event_handler;
		m_vt = vtable;
	}

private:
	void* r_eh;
	EventHandlerVtable m_vt;

	Model::Palette const* r_pal;

	GtkDrawingArea* m_handle;
	static gboolean draw_callback(GtkWidget* widget, cairo_t* cr, gpointer self)
	{
		auto w = gtk_widget_get_allocated_width(widget);
		auto h = gtk_widget_get_allocated_height(widget);
		reinterpret_cast<Impl*>(self)->render(Geom::Dimension{}.width(w).height(h), cr);
		return FALSE;
	}

	static gboolean on_mouse_down(GtkWidget*, GdkEvent* e, gpointer self)
	{
		auto& obj = *reinterpret_cast<Impl*>(self);
		if(obj.r_eh != nullptr)
		{
			auto event = reinterpret_cast<GdkEventButton const*>(e);
			auto const index = static_cast<size_t>(event->x / ItemSize);
			obj.m_vt.m_on_mouse_down(obj.r_eh, obj, index, event->button);
			return FALSE;
		}
		return TRUE;
	}

	static gboolean on_mouse_up(GtkWidget*, GdkEvent* e, gpointer self)
	{
		auto& obj = *reinterpret_cast<Impl*>(self);
		if(obj.r_eh != nullptr)
		{
			auto event = reinterpret_cast<GdkEventButton const*>(e);
			auto const index = static_cast<size_t>(event->x / ItemSize);
			obj.m_vt.m_on_mouse_up(obj.r_eh, obj, index, event->button);
			return FALSE;
		}
		return TRUE;
	}

	static gboolean on_mouse_move(GtkWidget*, GdkEvent* e, gpointer self)
	{
		auto& obj = *reinterpret_cast<Impl*>(self);
		if(obj.r_eh != nullptr)
		{
			auto event = reinterpret_cast<GdkEventMotion const*>(e);
			auto const index = static_cast<size_t>(event->x / ItemSize);
			obj.m_vt.m_on_mouse_move(obj.r_eh, obj, index);
			return FALSE;
		}
		return TRUE;
	}
};

Texpainter::Ui::PaletteView::PaletteView(Container& cnt): m_impl{new Impl{cnt}}
{
}

Texpainter::Ui::PaletteView::~PaletteView()
{
	delete m_impl;
}

Texpainter::Ui::PaletteView& Texpainter::Ui::PaletteView::palette(Model::Palette const& pal)
{
	m_impl->palette(pal);
	return *this;
}

Texpainter::Ui::PaletteView&
Texpainter::Ui::PaletteView::eventHandler(void* event_handler, EventHandlerVtable const& vtable)
{
	m_impl->eventHandler(event_handler, vtable);
	return *this;
}

Texpainter::Ui::PaletteView& Texpainter::Ui::PaletteView::update()
{
	m_impl->update();
	return *this;
}
