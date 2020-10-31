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
			ret[k] = static_cast<float>(
			    255.0 * std::pow(static_cast<double>(k) / (ret.size() - 1), 1 / 2.2));
		}

		return ret;
	}
	constexpr auto gamma_22 = gen_gamma_22_lut();
}

class Texpainter::Ui::ImageView::Impl: private ImageView
{
public:
	explicit Impl(Container& cnt): ImageView{*this}, m_size_current{0, 0}
	{
		auto widget = gtk_drawing_area_new();
		g_object_ref_sink(widget);
		cnt.add(widget);
		m_handle            = GTK_DRAWING_AREA(widget);
		m_img_surface       = nullptr;
		r_eh                = nullptr;
		m_emit_mouse_events = false;
		gtk_widget_add_events(widget,
		                      GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK
		                          | GDK_BUTTON_RELEASE_MASK | GDK_SCROLL_MASK);
		g_signal_connect(G_OBJECT(widget), "draw", G_CALLBACK(draw_callback), this);
		g_signal_connect(G_OBJECT(widget), "button-press-event", G_CALLBACK(on_mouse_down), this);
		g_signal_connect(G_OBJECT(widget), "button-release-event", G_CALLBACK(on_mouse_up), this);
		g_signal_connect(G_OBJECT(widget), "motion-notify-event", G_CALLBACK(on_mouse_move), this);

		m_background      = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 32, 32);
		auto const stride = cairo_image_surface_get_stride(m_background);
		cairo_surface_flush(m_background);
		auto const data = cairo_image_surface_get_data(m_background);
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
		cairo_surface_mark_dirty(m_background);
	}

	~Impl()
	{
		if(m_img_surface != nullptr) { cairo_surface_destroy(m_img_surface); }
		gtk_widget_destroy(GTK_WIDGET(m_handle));
		g_object_unref(m_handle);
		m_impl = nullptr;
		cairo_surface_destroy(m_background);
	}

	void render(Size2d dim, cairo_t* cr)
	{
		cairo_set_source_surface(cr, m_background, 0.0, 0.0);
		cairo_pattern_set_extend(cairo_get_source(cr), CAIRO_EXTEND_REPEAT);
		cairo_rectangle(cr, 0.0, 0.0, dim.width(), dim.height());
		cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
		cairo_fill(cr);

		if(m_img_surface != nullptr) [[likely]]
			{
				cairo_set_source_surface(cr, m_img_surface, 0.0, 0.0);
				cairo_pattern_set_extend(cairo_get_source(cr), CAIRO_EXTEND_REPEAT);
				cairo_rectangle(cr, 0.0, 0.0, dim.width(), dim.height());
				cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
				cairo_fill(cr);
			}
	}

	void image(PixelStore::Image const& img)
	{
		if(img.size() != m_size_current || m_img_surface == nullptr) [[unlikely]]
			{
				auto surface =
				    cairo_image_surface_create(CAIRO_FORMAT_ARGB32, img.width(), img.height());
				if(cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) { return; }

				if(surface == nullptr) { abort(); }

				cairo_surface_destroy(m_img_surface);
				m_img_surface  = surface;
				m_size_current = img.size();
			}
		update(img);
	}

	void clear()
	{
		if(m_img_surface != nullptr)
		{
			cairo_surface_destroy(m_img_surface);
			m_img_surface  = nullptr;
			m_size_current = Size2d{0, 0};
		}
	}

	void eventHandler(void* event_handler, EventHandlerVtable const& vtable)
	{
		r_eh = event_handler;
		m_vt = vtable;
	}

	void minSize(Size2d size)
	{
		gtk_widget_set_size_request(GTK_WIDGET(m_handle), size.width(), size.height());
	}

	Size2d imageSize() const noexcept { return m_size_current; }

	void focus() { gtk_widget_grab_focus(GTK_WIDGET(m_handle)); }

	void alwaysEmitMouseEvents(bool status) { m_emit_mouse_events = status; }

private:
	void* r_eh;
	EventHandlerVtable m_vt;
	bool m_emit_mouse_events;
	cairo_surface_t* m_background;

	cairo_surface_t* m_img_surface;
	Size2d m_size_current;

	void update(PixelStore::Image const& img)
	{
		auto const stride = cairo_image_surface_get_stride(m_img_surface);
		cairo_surface_flush(m_img_surface);
		auto const data = cairo_image_surface_get_data(m_img_surface);
		assert(data != nullptr);
		auto const w  = img.width();
		auto const h  = img.height();
		auto read_ptr = std::data(img.pixels());
		for(uint32_t row = 0; row < h; ++row)
		{
			auto write_ptr = data + row * stride;
			for(uint32_t col = 0; col < w; ++col)
			{
				constexpr auto last_lut_entry = static_cast<int>(gamma_22.size() - 1);
				auto val                      = chooseValIfInRange(
                    read_ptr->value(),
                    row % 3 == 0 ? vec4_t{0.0f, 0.0f, 0.0f, 0.0f} : vec4_t{1.0f, 1.0f, 1.0f, 1.0f},
                    row % 3 != 0 ? vec4_t{0.0f, 0.0f, 0.0f, 0.0f} : vec4_t{1.0f, 1.0f, 1.0f, 1.0f},
                    col % 3 != 0 ? vec4_t{0.0f, 0.0f, 0.0f, 0.0f} : vec4_t{1.0f, 1.0f, 1.0f, 1.0f});

				auto pixel_out = static_cast<float>(last_lut_entry) * val;

				auto as_ints = vec4i_t{static_cast<int>(pixel_out[0]),
				                       static_cast<int>(pixel_out[1]),
				                       static_cast<int>(pixel_out[2]),
				                       static_cast<int>(pixel_out[3])};

				write_ptr[0] = gamma_22[as_ints[2]];
				write_ptr[1] = gamma_22[as_ints[1]];
				write_ptr[2] = gamma_22[as_ints[0]];
				write_ptr[3] = gamma_22[as_ints[3]];

				write_ptr += 4;
				++read_ptr;
			}
		}
		cairo_surface_mark_dirty(m_img_surface);
		gtk_widget_queue_draw(GTK_WIDGET(m_handle));
	}


	GtkDrawingArea* m_handle;
	static gboolean draw_callback(GtkWidget* widget, cairo_t* cr, gpointer self)
	{
		auto const w = static_cast<uint32_t>(gtk_widget_get_allocated_width(widget));
		auto const h = static_cast<uint32_t>(gtk_widget_get_allocated_height(widget));
		reinterpret_cast<Impl*>(self)->render(Size2d{w, h}, cr);
		return FALSE;
	}

	static gboolean on_mouse_down(GtkWidget*, GdkEvent* e, gpointer self)
	{
		auto& obj = *reinterpret_cast<Impl*>(self);
		if(obj.r_eh != nullptr)
		{
			auto event_button = reinterpret_cast<GdkEventButton const*>(e);
			obj.m_vt.m_on_mouse_down(obj.r_eh,
			                         obj,
			                         vec2_t{event_button->x, event_button->y},
			                         vec2_t{event_button->x_root, event_button->y_root},
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
			                       vec2_t{event_button->x, event_button->y},
			                       vec2_t{event_button->x_root, event_button->y_root},
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
			                         vec2_t{event_button->x, event_button->y},
			                         vec2_t{event_button->x_root, event_button->y_root});
			return FALSE;
		}
		return TRUE;
	}
};

Texpainter::Ui::ImageView::ImageView(Container& cnt): m_impl{new Impl{cnt}} {}

Texpainter::Ui::ImageView::~ImageView() { delete m_impl; }

Texpainter::Ui::ImageView& Texpainter::Ui::ImageView::image(PixelStore::Image const& img)
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