//@	{"targets":[{"name":"image_view.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./image_view.hpp"
#include "pixel_store/image.hpp"

#include <gtk/gtk.h>

#include <cassert>

namespace
{
	constexpr auto gen_gamma_22_lut()
	{
		std::array<uint8_t, 42868> ret{};
		for(int k = 0; k < static_cast<int>(ret.size()); ++k)
		{
			ret[k] = static_cast<uint8_t>(
			    255.0 * std::pow(static_cast<double>(k) / (ret.size() - 1), 1 / 2.2));
		}

		return ret;
	}
	constexpr auto gamma_22 = gen_gamma_22_lut();

	struct CairoSurfaceDeleter
	{
		void operator()(cairo_surface_t* surface)
		{
			if(surface != nullptr) { cairo_surface_destroy(surface); }
		}
	};

	class CairoSurface
	{
		using Handle = std::unique_ptr<cairo_surface_t, CairoSurfaceDeleter>;

	public:
		CairoSurface(): m_size_current{0, 0} {}

		explicit CairoSurface(Texpainter::Size2d size): m_size_current{size}
		{
			using Texpainter::Size2d;
			if(m_size_current == Size2d{0, 0}) { return; }

			m_img_surface = Handle{cairo_image_surface_create(
			    CAIRO_FORMAT_ARGB32, m_size_current.width(), m_size_current.height())};

			auto surface = m_img_surface.get();
			if(cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS)
			{
				m_size_current = Size2d{0, 0};
				m_img_surface.reset();
				return;
			}
		}

		explicit CairoSurface(Texpainter::Span2d<Texpainter::PixelStore::RgbaValue const> img)
		    : CairoSurface{img.size()}
		{
			using Texpainter::chooseValIfInRange;
			using Texpainter::vec4_t;
			using Texpainter::vec4i_t;

			auto surface = m_img_surface.get();
			if(surface == nullptr) { return; }

			auto const stride = cairo_image_surface_get_stride(surface);
			cairo_surface_flush(surface);
			auto const data = cairo_image_surface_get_data(surface);
			assert(data != nullptr);
			auto const w  = img.width();
			auto const h  = img.height();
			auto read_ptr = std::data(img);
			for(uint32_t row = 0; row < h; ++row)
			{
				auto write_ptr = data + row * stride;
				for(uint32_t col = 0; col < w; ++col)
				{
					constexpr auto last_lut_entry = static_cast<int>(gamma_22.size() - 1);
					auto val                      = chooseValIfInRange(read_ptr->value(),
                                                  row % 3 == 0 ? vec4_t{0.0f, 0.0f, 0.0f, 0.0f}
                                                               : vec4_t{1.0f, 1.0f, 1.0f, 1.0f},
                                                  row % 3 != 0 ? vec4_t{0.0f, 0.0f, 0.0f, 0.0f}
                                                               : vec4_t{1.0f, 1.0f, 1.0f, 1.0f},
                                                  col % 3 != 0 ? vec4_t{0.0f, 0.0f, 0.0f, 0.0f}
                                                               : vec4_t{1.0f, 1.0f, 1.0f, 1.0f});

					auto pixel_out = static_cast<float>(last_lut_entry) * val;

					auto as_ints = vec4i_t{static_cast<int>(pixel_out[0]),
					                       static_cast<int>(pixel_out[1]),
					                       static_cast<int>(pixel_out[2]),
					                       static_cast<int>(pixel_out[3])};

					write_ptr[0] = gamma_22[as_ints[2]];
					write_ptr[1] = gamma_22[as_ints[1]];
					write_ptr[2] = gamma_22[as_ints[0]];
					write_ptr[3] = static_cast<uint8_t>(255.0f * read_ptr->alpha());

					write_ptr += 4;
					++read_ptr;
				}
			}
			cairo_surface_mark_dirty(surface);
		}

		cairo_surface_t* get() const { return m_img_surface.get(); }

		Texpainter::Size2d size() const { return m_size_current; }

	private:
		Handle m_img_surface;
		Texpainter::Size2d m_size_current;
	};

	CairoSurface createBackground()
	{
		CairoSurface ret{Texpainter::Size2d{32, 32}};

		auto surface      = ret.get();
		auto const stride = cairo_image_surface_get_stride(surface);
		cairo_surface_flush(surface);
		auto const data = cairo_image_surface_get_data(surface);
		for(uint32_t row = 0; row < 32; ++row)
		{
			auto write_ptr = data + row * stride;
			for(uint32_t col = 0; col < 32; ++col)
			{
				constexpr auto last_lut_entry = static_cast<int>(gamma_22.size() - 1);
				constexpr auto intensity_a = gamma_22[static_cast<int>(9.0 * last_lut_entry / 16)];
				constexpr auto intensity_b = gamma_22[static_cast<int>(7.0 * last_lut_entry / 16)];

				auto const i =
				    (col < 16 && row < 16) || (col >= 16 && row >= 16) ? intensity_a : intensity_b;

				write_ptr[0] = i;
				write_ptr[1] = i;
				write_ptr[2] = i;
				write_ptr[3] = 255;

				write_ptr += 4;
			}
		}
		cairo_surface_mark_dirty(surface);

		return ret;
	}

	struct Overlay
	{
		CairoSurface surface;
		Texpainter::vec2_t location;
	};
}

class Texpainter::Ui::ImageView::Impl: private ImageView
{
public:
	explicit Impl(Container& cnt): ImageView{*this}, m_min_size{160, 120}, m_scale{1.0}
	{
		m_scrolled_window = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(nullptr, nullptr));
		gtk_widget_set_size_request(
		    GTK_WIDGET(m_scrolled_window), m_min_size.width(), m_min_size.height());
		cnt.add(m_scrolled_window);
		auto widget = gtk_drawing_area_new();
		g_object_ref_sink(widget);
		m_handle = GTK_DRAWING_AREA(widget);
		gtk_container_add(GTK_CONTAINER(m_scrolled_window), widget);
		r_eh                = nullptr;
		m_emit_mouse_events = false;
		gtk_widget_add_events(widget,
		                      GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK
		                          | GDK_BUTTON_RELEASE_MASK | GDK_SCROLL_MASK);
		g_signal_connect(G_OBJECT(widget), "draw", G_CALLBACK(draw_callback), this);
		g_signal_connect(G_OBJECT(widget), "button-press-event", G_CALLBACK(on_mouse_down), this);
		g_signal_connect(G_OBJECT(widget), "button-release-event", G_CALLBACK(on_mouse_up), this);
		g_signal_connect(G_OBJECT(widget), "motion-notify-event", G_CALLBACK(on_mouse_move), this);
		g_signal_connect(G_OBJECT(widget), "scroll-event", G_CALLBACK(on_scroll), this);

		m_background = createBackground();
	}

	~Impl()
	{
		gtk_widget_destroy(GTK_WIDGET(m_handle));
		g_object_unref(m_handle);
		gtk_widget_destroy(GTK_WIDGET(m_scrolled_window));
		m_impl = nullptr;
	}

	void render(Size2d dim, cairo_t* cr) const
	{
		cairo_scale(cr, m_scale, m_scale);
		cairo_set_source_surface(cr, m_background.get(), 0.0, 0.0);
		cairo_pattern_set_extend(cairo_get_source(cr), CAIRO_EXTEND_REPEAT);
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_NEAREST);
		cairo_rectangle(cr, 0.0, 0.0, dim.width() / m_scale, dim.height() / m_scale);
		cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
		cairo_fill(cr);

		if(auto img = m_img_surface.get(); img != nullptr)
		{
			cairo_set_source_surface(cr, img, 0.0, 0.0);
			cairo_pattern_set_extend(cairo_get_source(cr), CAIRO_EXTEND_REPEAT);
			cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_NEAREST);
			cairo_rectangle(cr, 0.0, 0.0, dim.width() / m_scale, dim.height() / m_scale);
			cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
			cairo_fill(cr);
		}

		if(auto img = m_overlay.surface.get(); img != nullptr)
		{
			cairo_set_source_surface(cr, img, m_overlay.location[0], m_overlay.location[1]);
			cairo_pattern_set_extend(cairo_get_source(cr), CAIRO_EXTEND_NONE);
			cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_NEAREST);
			auto const surf_rect = m_overlay.surface.size();
			cairo_rectangle(cr,
			                m_overlay.location[0],
			                m_overlay.location[1],
			                surf_rect.width(),
			                surf_rect.height());
			cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
			cairo_fill(cr);
		}
	}

	void image(Span2d<PixelStore::RgbaValue const> img)
	{
		m_img_surface = CairoSurface{img};
		auto size     = img.size();
		gtk_widget_set_size_request(GTK_WIDGET(m_handle),
		                            static_cast<int>(m_scale * size.width()),
		                            static_cast<int>(m_scale * size.height()));
		gtk_widget_queue_draw(GTK_WIDGET(m_handle));
	}

	void clear()
	{
		m_img_surface = CairoSurface{};
		m_overlay     = Overlay{};

		gtk_widget_set_size_request(GTK_WIDGET(m_handle), m_min_size.width(), m_min_size.height());
		gtk_widget_queue_draw(GTK_WIDGET(m_handle));
	}

	void eventHandler(void* event_handler, EventHandlerVtable const& vtable)
	{
		r_eh = event_handler;
		m_vt = vtable;
	}

	void minSize(Size2d size)
	{
		m_min_size = size;
		gtk_widget_set_size_request(GTK_WIDGET(m_handle), size.width(), size.height());
		gtk_widget_set_size_request(GTK_WIDGET(m_scrolled_window), size.width(), size.height());
	}

	Size2d imageSize() const noexcept { return m_img_surface.size(); }

	void focus() { gtk_widget_grab_focus(GTK_WIDGET(m_handle)); }

	void alwaysEmitMouseEvents(bool status) { m_emit_mouse_events = status; }

	void overlay(Span2d<PixelStore::RgbaValue const> img, vec2_t location)
	{
		m_overlay = Overlay{CairoSurface{img}, location};
		m_overlay.location -= 0.5
		                      * vec2_t{static_cast<double>(m_overlay.surface.size().width()),
		                               static_cast<double>(m_overlay.surface.size().height())};
	}

	void overlayLocation(vec2_t location)
	{
		m_overlay.location = location
		                     - 0.5
		                           * vec2_t{static_cast<double>(m_overlay.surface.size().width()),
		                                    static_cast<double>(m_overlay.surface.size().height())};
		gtk_widget_queue_draw(GTK_WIDGET(m_handle));
	}

	ImageView& scale(double factor)
	{
		m_scale         = factor;
		auto const size = m_img_surface.size();
		gtk_widget_set_size_request(GTK_WIDGET(m_handle),
		                            static_cast<int>(m_scale * size.width()),
		                            static_cast<int>(m_scale * size.height()));
		return *this;
	}

	void scrollTo(vec2_t loc)
	{
		while(gtk_events_pending())
		{
			gtk_main_iteration();
		}

		auto const hadj       = gtk_scrolled_window_get_hadjustment(m_scrolled_window);
		auto const vadj       = gtk_scrolled_window_get_vadjustment(m_scrolled_window);
		auto const hadj_lower = gtk_adjustment_get_lower(hadj);
		auto const vadj_lower = gtk_adjustment_get_lower(vadj);
		auto const hadj_upper = gtk_adjustment_get_upper(hadj) - gtk_adjustment_get_page_size(hadj);
		auto const vadj_upper = gtk_adjustment_get_upper(vadj) - gtk_adjustment_get_page_size(vadj);

		auto const lower = vec2_t{hadj_lower, vadj_lower};
		auto const upper = vec2_t{hadj_upper, vadj_upper};

		auto const diff = upper - lower;
		auto const size = m_img_surface.size();
		auto const loc_normalized =
		    loc / vec2_t{static_cast<double>(size.width()), static_cast<double>(size.height())};
		auto const loc_new = lower + diff * loc_normalized;

		gtk_adjustment_set_value(hadj, loc_new[0]);
		gtk_adjustment_set_value(vadj, loc_new[1]);
	}

private:
	void* r_eh;
	EventHandlerVtable m_vt;
	bool m_emit_mouse_events;
	CairoSurface m_background;
	CairoSurface m_img_surface;
	Overlay m_overlay;
	Size2d m_min_size;
	double m_scale;

	GtkScrolledWindow* m_scrolled_window;
	GtkDrawingArea* m_handle;

	static gboolean draw_callback(GtkWidget* widget, cairo_t* cr, gpointer self)
	{
		auto const w = static_cast<uint32_t>(gtk_widget_get_allocated_width(widget));
		auto const h = static_cast<uint32_t>(gtk_widget_get_allocated_height(widget));
		reinterpret_cast<Impl*>(self)->render(Size2d{w, h}, cr);
		return TRUE;
	}

	static gboolean on_mouse_down(GtkWidget*, GdkEvent* e, gpointer self)
	{
		auto& obj = *reinterpret_cast<Impl*>(self);
		if(obj.r_eh != nullptr)
		{
			auto event_button = reinterpret_cast<GdkEventButton const*>(e);
			obj.m_vt.m_on_mouse_down(obj.r_eh,
			                         obj,
			                         vec2_t{event_button->x, event_button->y} / obj.m_scale,
			                         vec2_t{event_button->x_root, event_button->y_root}
			                             / obj.m_scale,
			                         event_button->button);
			return TRUE;
		}
		return FALSE;
	}

	static gboolean on_mouse_up(GtkWidget*, GdkEvent* e, gpointer self)
	{
		auto& obj = *reinterpret_cast<Impl*>(self);
		if(obj.r_eh != nullptr)
		{
			auto event_button = reinterpret_cast<GdkEventButton const*>(e);
			obj.m_vt.m_on_mouse_up(obj.r_eh,
			                       obj,
			                       vec2_t{event_button->x, event_button->y} / obj.m_scale,
			                       vec2_t{event_button->x_root, event_button->y_root} / obj.m_scale,
			                       event_button->button);
			return TRUE;
		}
		return FALSE;
	}

	static gboolean on_mouse_move(GtkWidget*, GdkEvent* e, gpointer self)
	{
		auto& obj = *reinterpret_cast<Impl*>(self);
		if(obj.r_eh != nullptr)
		{
			auto event_button = reinterpret_cast<GdkEventMotion const*>(e);
			obj.m_vt.m_on_mouse_move(obj.r_eh,
			                         obj,
			                         vec2_t{event_button->x, event_button->y} / obj.m_scale,
			                         vec2_t{event_button->x_root, event_button->y_root}
			                             / obj.m_scale);
			return TRUE;
		}
		return FALSE;
	}

	static gboolean on_scroll(GtkWidget*, GdkEvent* e, gpointer self)
	{
		auto& obj = *reinterpret_cast<Impl*>(self);
		if(obj.r_eh != nullptr)
		{
			auto event_scroll = reinterpret_cast<GdkEventScroll const*>(e);
			vec2_t dir{};
			switch(event_scroll->direction)
			{
				case GDK_SCROLL_UP: dir = vec2_t{0, 1}; break;
				case GDK_SCROLL_DOWN: dir = vec2_t{0, -1}; break;
				case GDK_SCROLL_LEFT: dir = vec2_t{-1, 0}; break;
				case GDK_SCROLL_RIGHT: dir = vec2_t{1, 0}; break;
				case GDK_SCROLL_SMOOTH: gdk_event_get_scroll_deltas(e, &dir[0], &dir[1]); break;
			}
			obj.m_vt.m_on_scroll(obj.r_eh, obj, dir);
			return TRUE;
		}
		return FALSE;
	}
};

Texpainter::Ui::ImageView::ImageView(Container& cnt): m_impl{new Impl{cnt}} {}

Texpainter::Ui::ImageView::~ImageView() { delete m_impl; }

Texpainter::Ui::ImageView& Texpainter::Ui::ImageView::image(Span2d<PixelStore::RgbaValue const> img)
{
	m_impl->image(img);
	return *this;
}

Texpainter::Ui::ImageView& Texpainter::Ui::ImageView::eventHandler(void* event_handler,
                                                                   EventHandlerVtable const& vtable)
{
	m_impl->eventHandler(event_handler, vtable);
	return *this;
}

Texpainter::Ui::ImageView& Texpainter::Ui::ImageView::minSize(Size2d size)
{
	m_impl->minSize(size);
	return *this;
}

Texpainter::Size2d Texpainter::Ui::ImageView::imageSize() const noexcept
{
	return m_impl->imageSize();
}

Texpainter::Ui::ImageView& Texpainter::Ui::ImageView::focus()
{
	m_impl->focus();
	return *this;
}

Texpainter::Ui::ImageView& Texpainter::Ui::ImageView::alwaysEmitMouseEvents(bool)
{
	m_impl->alwaysEmitMouseEvents(true);
	return *this;
}

Texpainter::Ui::ImageView& Texpainter::Ui::ImageView::clear()
{
	m_impl->clear();
	return *this;
}

Texpainter::Ui::ImageView& Texpainter::Ui::ImageView::overlay(
    Span2d<PixelStore::RgbaValue const> img, vec2_t initial_location)
{
	m_impl->overlay(img, initial_location);
	return *this;
}

Texpainter::Ui::ImageView& Texpainter::Ui::ImageView::overlayLocation(vec2_t pos)
{
	m_impl->overlayLocation(pos);
	return *this;
}

Texpainter::Ui::ImageView& Texpainter::Ui::ImageView::scale(double factor)
{
	m_impl->scale(factor);
	return *this;
}

Texpainter::Ui::ImageView& Texpainter::Ui::ImageView::scrollTo(vec2_t loc)
{
	m_impl->scrollTo(loc);
	return *this;
}