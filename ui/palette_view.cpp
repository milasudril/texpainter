//@	{"targets":[{"name":"palette_view.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./palette_view.hpp"
#include "model/pixel.hpp"

#include <gtk/gtk.h>

#include <cassert>

namespace
{
	constexpr Texpainter::Model::BasicPixel<Texpainter::Model::ColorProfiles::Gamma22> toRgbGamma22(
	    Texpainter::Ui::PaletteView::HighlightMode mode)
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
	explicit Impl(Container& cnt)
	    : PaletteView{*this}
	    , m_min_size{Size2d{32, 32}}
	    , m_n_cols{1}
	    , m_n_rows{1}
	    , m_colors{0}
	    , m_highlight_mode{0}
	{
		auto widget = gtk_drawing_area_new();
		g_object_ref_sink(widget);
		cnt.add(widget);
		m_handle = GTK_DRAWING_AREA(widget);
		r_eh     = nullptr;
		gtk_widget_add_events(widget,
		                      GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK
		                          | GDK_BUTTON_RELEASE_MASK | GDK_KEY_PRESS_MASK | GDK_SCROLL_MASK);
		g_signal_connect(G_OBJECT(widget), "draw", G_CALLBACK(draw_callback), this);
		g_signal_connect(G_OBJECT(widget), "size-allocate", G_CALLBACK(size_callback), this);

		g_signal_connect(G_OBJECT(widget), "button-press-event", G_CALLBACK(on_mouse_down), this);
		g_signal_connect(G_OBJECT(widget), "button-release-event", G_CALLBACK(on_mouse_up), this);
		g_signal_connect(G_OBJECT(widget), "motion-notify-event", G_CALLBACK(on_mouse_move), this);

		// TODO: Share with ImageView
		m_background      = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 32, 32);
		auto const stride = cairo_image_surface_get_stride(m_background);
		cairo_surface_flush(m_background);
		auto const data = cairo_image_surface_get_data(m_background);
		for(uint32_t row = 0; row < 32; ++row)
		{
			auto write_ptr = data + row * stride;
			for(uint32_t col = 0; col < 32; ++col)
			{
				constexpr auto intensity_a = static_cast<uint8_t>(196);
				constexpr auto intensity_b = static_cast<uint8_t>(175);

				auto const i =
				    (col < 16 && row < 16) || (col >= 16 && row >= 16) ? intensity_a : intensity_b;

				write_ptr[0] = i;
				write_ptr[1] = i;
				write_ptr[2] = i;
				write_ptr[3] = 255;

				write_ptr += 4;
			}
		}
		cairo_surface_mark_dirty(m_background);
	}

	~Impl()
	{
		gtk_widget_destroy(GTK_WIDGET(m_handle));
		g_object_unref(m_handle);
		m_impl = nullptr;
		cairo_surface_destroy(m_background);
	}

	void render(Size2d dim, cairo_t* cr)
	{
		cairo_set_source_surface(cr, m_background, 0.0, 0.0);
		cairo_pattern_set_extend(cairo_get_source(cr), CAIRO_EXTEND_REPEAT);
		cairo_rectangle(cr,
		                0.0,
		                0.0,
		                m_n_cols * (dim.width() / m_n_cols),
		                m_n_rows * (dim.height() / m_n_rows));
		cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
		cairo_fill(cr);

		cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
		std::ranges::for_each(
		    m_colors,
		    [cr,
		     item_width  = dim.width() / m_n_cols - 4,
		     item_height = dim.height() / m_n_rows - 4,
		     cols        = m_n_cols,
		     k           = 0](auto color) mutable {
			    color *= Model::Pixel{color.alpha(), color.alpha(), color.alpha(), 1.0f};
			    auto const color_conv = Model::BasicPixel<Model::ColorProfiles::Gamma22>{color};
			    cairo_set_source_rgba(cr,
			                          color_conv.red(),
			                          color_conv.green(),
			                          color_conv.blue(),
			                          color_conv.alpha());
			    cairo_rectangle(cr,
			                    (k % cols) * (item_width + 4) + 2,
			                    (k / cols) * (item_height + 4) + 2,
			                    item_width,
			                    item_height);
			    cairo_fill(cr);
			    ++k;
		    });

		cairo_set_line_width(cr, 2);
		std::ranges::for_each(m_highlight_mode,
		                      [cr,
		                       item_width  = dim.width() / m_n_cols - 2,
		                       item_height = dim.height() / m_n_rows - 2,
		                       cols        = m_n_cols,
		                       k           = 0](auto mode) mutable {
			                      auto const color_conv = toRgbGamma22(mode);
			                      cairo_set_source_rgba(cr,
			                                            color_conv.red(),
			                                            color_conv.green(),
			                                            color_conv.blue(),
			                                            color_conv.alpha());
			                      cairo_rectangle(cr,
			                                      (k % cols) * (item_width + 2) + 1,
			                                      (k / cols) * (item_height + 2) + 1,
			                                      item_width,
			                                      item_height);
			                      cairo_stroke(cr);
			                      ++k;
		                      });
	}

	void palette(std::span<Model::Pixel const> pal)
	{
		m_colors         = Model::Palette{pal};
		m_highlight_mode = DataBlock<HighlightMode>{std::size(m_colors)};
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

	void highlightMode(Model::ColorIndex index, HighlightMode mode)
	{
		if(index.value() < m_colors.size()) { *(m_highlight_mode.begin() + index.value()) = mode; }
	}

	void minSize(Size2d size)
	{
		m_min_size = size;
		recalculateWidgetSize();
	}

	Model::Pixel color(Model::ColorIndex index) const { return m_colors[index]; }

	void color(Model::ColorIndex index, Model::Pixel value)
	{
		if(index.value() < m_colors.size())
		{
			m_colors[index] = value;
			update();
		}
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
	cairo_surface_t* m_background;

	void recalculateWidgetSize()
	{
		if(m_colors.size() == 0) [[unlikely]]
			{
				return;
			}

		auto widget = GTK_WIDGET(m_handle);
		auto w      = static_cast<uint32_t>(gtk_widget_get_allocated_width(widget));
		if(w < m_min_size.width())
		{
			w = m_min_size.width();
			gtk_widget_set_size_request(widget, w, -1);
		}
		auto width_item = 0.0f;
		auto div        = m_colors.size();
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

	Model::ColorIndex coordsToItem(vec2_t loc) const
	{
		auto const widget = GTK_WIDGET(m_handle);
		auto const widget_size =
		    vec2_t{static_cast<double>(gtk_widget_get_allocated_width(widget)),
		           static_cast<double>(gtk_widget_get_allocated_height(widget))};

		auto const item_size =
		    widget_size / vec2_t{static_cast<double>(m_n_cols), static_cast<double>(m_n_rows)};
		auto const col_row = loc / item_size;

		auto const col = static_cast<int>(col_row[0]);
		auto const row = static_cast<int>(col_row[1]);

		return Model::ColorIndex{
		    std::min(m_colors.size(), static_cast<uint32_t>(m_n_cols * row + col))};
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
		reinterpret_cast<Impl*>(self)->render(
		    Size2d{static_cast<uint32_t>(w), static_cast<uint32_t>(h)}, cr);
		return FALSE;
	}

	static gboolean on_mouse_down(GtkWidget*, GdkEvent* e, gpointer self)
	{
		auto& obj = *reinterpret_cast<Impl*>(self);
		if(obj.r_eh != nullptr)
		{
			auto event       = reinterpret_cast<GdkEventButton const*>(e);
			auto const index = obj.coordsToItem(
			    vec2_t{static_cast<double>(event->x), static_cast<double>(event->y)});
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
			auto event       = reinterpret_cast<GdkEventButton const*>(e);
			auto const index = obj.coordsToItem(
			    vec2_t{static_cast<double>(event->x), static_cast<double>(event->y)});
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
			auto event       = reinterpret_cast<GdkEventMotion const*>(e);
			auto const index = obj.coordsToItem(
			    vec2_t{static_cast<double>(event->x), static_cast<double>(event->y)});
			obj.m_vt.m_on_mouse_move(obj.r_eh, obj, index);
			return FALSE;
		}
		return TRUE;
	}
};

Texpainter::Ui::PaletteView::PaletteView(Container& cnt): m_impl{new Impl{cnt}} {}

Texpainter::Ui::PaletteView::~PaletteView() { delete m_impl; }

Texpainter::Ui::PaletteView& Texpainter::Ui::PaletteView::palette(std::span<Model::Pixel const> pal)
{
	m_impl->palette(pal);
	return *this;
}

Texpainter::Ui::PaletteView& Texpainter::Ui::PaletteView::eventHandler(
    void* event_handler, EventHandlerVtable const& vtable)
{
	m_impl->eventHandler(event_handler, vtable);
	return *this;
}

Texpainter::Ui::PaletteView& Texpainter::Ui::PaletteView::update()
{
	m_impl->update();
	return *this;
}

Texpainter::Ui::PaletteView& Texpainter::Ui::PaletteView::highlightMode(Model::ColorIndex index,
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

Texpainter::Model::Pixel Texpainter::Ui::PaletteView::PaletteView::color(
    Model::ColorIndex index) const
{
	return m_impl->color(index);
}

Texpainter::Ui::PaletteView& Texpainter::Ui::PaletteView::color(Model::ColorIndex index,
                                                                Model::Pixel value)
{
	m_impl->color(index, value);
	return *this;
}