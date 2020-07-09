//@	{"targets":[{"name":"palette_view.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./palette_view.hpp"
#include "model/pixel.hpp"

#include <gtk/gtk.h>

#include <cassert>

namespace
{
	constexpr Texpainter::Model::BasicPixel<Texpainter::Model::ColorProfiles::Gamma22>
	color(Texpainter::Ui::PaletteView::HighlightMode mode)
	{
		switch(mode)
		{
			case Texpainter::Ui::PaletteView::HighlightMode::None:
				return Texpainter::Model::BasicPixel<Texpainter::Model::ColorProfiles::Gamma22>{
				   Texpainter::Model::Pixel{0.16f, 0.16f, 0.16f, 1.0f}};
			case Texpainter::Ui::PaletteView::HighlightMode::Read:
				return Texpainter::Model::BasicPixel<Texpainter::Model::ColorProfiles::Gamma22>{
				   Texpainter::Model::Pixel{0.0f, 0.5f, 0.0f, 0.5f}};
			case Texpainter::Ui::PaletteView::HighlightMode::Write:
				return Texpainter::Model::BasicPixel<Texpainter::Model::ColorProfiles::Gamma22>{
				   Texpainter::Model::Pixel{0.5f, 0.0f, 0.0f, 0.5f}};
			default: abort();
		}
	}
}
class Texpainter::Ui::PaletteView::Impl: private PaletteView
{
public:
	explicit Impl(Container& cnt):
	   PaletteView{*this},
	   m_min_size{Size2d{32, 32}},
	   m_n_cols{1},
	   m_n_rows{1},
	   m_colors{0},
	   m_highlight_mode{0}
	{
		auto widget = gtk_drawing_area_new();
		g_object_ref_sink(widget);
		cnt.add(widget);
		m_handle = GTK_DRAWING_AREA(widget);
		r_eh = nullptr;
		gtk_widget_add_events(widget,
		                      GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK
		                         | GDK_KEY_PRESS_MASK | GDK_SCROLL_MASK);
		g_signal_connect(G_OBJECT(widget), "draw", G_CALLBACK(draw_callback), this);
		g_signal_connect(G_OBJECT(widget), "size-allocate", G_CALLBACK(size_callback), this);

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

	void render(Size2d dim, cairo_t* cr)
	{
		auto context = gtk_widget_get_style_context(GTK_WIDGET(m_handle));
		gtk_render_background(context, cr, 0, 0, dim.width(), dim.height());
		std::ranges::for_each(
		   m_colors,
		   [cr,
		    item_width = dim.width() / m_n_cols - 4,
		    item_height = dim.height() / m_n_rows - 4,
		    cols = m_n_cols,
		    k = 0](auto color) mutable {
			   auto const color_conv = Model::BasicPixel<Model::ColorProfiles::Gamma22>{color};
			   cairo_set_source_rgba(
			      cr, color_conv.red(), color_conv.green(), color_conv.blue(), color_conv.alpha());
			   cairo_rectangle(cr,
			                   (k % cols) * (item_width + 4) + 2,
			                   (k / cols) * (item_height + 4) + 2,
			                   item_width,
			                   item_height);
			   cairo_fill(cr);
			   ++k;
		   });

		cairo_set_line_width(cr, 2);
		std::ranges::for_each(
		   m_highlight_mode,
		   [cr,
		    item_width = dim.width() / m_n_cols - 2,
		    item_height = dim.height() / m_n_rows - 2,
		    cols = m_n_cols,
		    k = 0](auto mode) mutable {
			   auto const color_conv = color(mode);
			   cairo_set_source_rgba(
			      cr, color_conv.red(), color_conv.green(), color_conv.blue(), color_conv.alpha());
			   cairo_rectangle(cr,
			                   (k % cols) * (item_width + 2) + 1,
			                   (k / cols) * (item_height + 2) + 1,
			                   item_width,
			                   item_height);
			   cairo_stroke(cr);
			   ++k;
		   });
	}

	void palette(Model::Palette const& pal)
	{
		m_colors = pal;
		m_highlight_mode = DataBlock<HighlightMode>{std::size(pal)};
		std::ranges::fill(m_highlight_mode, HighlightMode::None);
		recalculateWidgetSize();
	}

	void update()
	{
		auto widget = GTK_WIDGET(m_handle);
		gtk_widget_queue_draw(widget);
	}

	void eventHandler(void* event_handler, EventHandlerVtable const& vtable)
	{
		r_eh = event_handler;
		m_vt = vtable;
	}

	void highlightMode(size_t index, HighlightMode mode)
	{
		*(m_highlight_mode.begin() + index) = mode;
	}

	void minSize(Size2d size)
	{
		m_min_size = size;
		recalculateWidgetSize();
	}


private:
	void* r_eh;
	EventHandlerVtable m_vt;

	Size2d m_min_size;
	int m_n_cols;
	int m_n_rows;
	Model::Palette m_colors;
	DataBlock<HighlightMode> m_highlight_mode;

	GtkDrawingArea* m_handle;

	void recalculateWidgetSize()
	{
		if(m_colors.size() == 0) [[unlikely]]
			{
				return;
			}

		auto widget = GTK_WIDGET(m_handle);
		auto w = static_cast<uint32_t>(gtk_widget_get_allocated_width(widget));
		if(w < m_min_size.width())
		{
			w = m_min_size.width();
			gtk_widget_set_size_request(widget, w, -1);
		}
		auto width_item = 0.0f;
		auto div = m_colors.size();
		while(width_item < m_min_size.width())
		{
			width_item = static_cast<float>(w) / static_cast<float>(div);
			--div;
		}
		++div;
		m_n_cols = div;
		m_n_rows = m_colors.size() / div + ((m_colors.size() % div) != 0);
		gtk_widget_set_size_request(widget, -1, m_n_rows * m_min_size.height());
	}

	size_t coordsToItem(vec2_t pos) const
	{
		auto const widget = GTK_WIDGET(m_handle);
		auto const widget_size = vec2_t{static_cast<double>(gtk_widget_get_allocated_width(widget)),
		                                static_cast<double>(gtk_widget_get_allocated_height(widget))};

		auto const item_size =
		   widget_size / vec2_t{static_cast<double>(m_n_cols), static_cast<double>(m_n_rows)};
		auto const col_row = pos / item_size;

		auto const col = static_cast<int>(col_row[0]);
		auto const row = static_cast<int>(col_row[1]);

		return std::min(m_colors.size(), static_cast<uint32_t>(m_n_cols * row + col));
	}

	static void size_callback(GtkWidget* widget, GdkRectangle* allocation, gpointer self)
	{
		reinterpret_cast<Impl*>(self)->recalculateWidgetSize();
		reinterpret_cast<Impl*>(self)->update();
	}

	static gboolean draw_callback(GtkWidget* widget, cairo_t* cr, gpointer self)
	{
		auto w = gtk_widget_get_allocated_width(widget);
		auto h = gtk_widget_get_allocated_height(widget);
		reinterpret_cast<Impl*>(self)->render(Size2d{static_cast<uint32_t>(w), static_cast<uint32_t>(h)},
		                                      cr);
		return FALSE;
	}

	static gboolean on_mouse_down(GtkWidget*, GdkEvent* e, gpointer self)
	{
		auto& obj = *reinterpret_cast<Impl*>(self);
		if(obj.r_eh != nullptr)
		{
			auto event = reinterpret_cast<GdkEventButton const*>(e);
			auto const index =
			   obj.coordsToItem(vec2_t{static_cast<double>(event->x), static_cast<double>(event->y)});
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
			auto const index =
			   obj.coordsToItem(vec2_t{static_cast<double>(event->x), static_cast<double>(event->y)});
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
			auto const index =
			   obj.coordsToItem(vec2_t{static_cast<double>(event->x), static_cast<double>(event->y)});
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

Texpainter::Ui::PaletteView& Texpainter::Ui::PaletteView::highlightMode(size_t index,
                                                                        HighlightMode mode)
{
	m_impl->highlightMode(index, mode);
	return *this;
}

Texpainter::Ui::PaletteView& Texpainter::Ui::PaletteView::minSize(Size2d size)
{
	m_impl->minSize(size);
	return *this;
}