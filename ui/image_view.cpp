//@	{"targets":[{"name":"image_view.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./image_view.hpp"
#include "model/image.hpp"
#include "geom/dimension.hpp"
#include "utils/snap.hpp"

#include <gtk/gtk.h>

#include <cassert>

namespace
{
	constexpr auto gen_gamma_22_lut()
	{
		float input_vals[256] = {};
		for(int k = 0; k < 256; ++k)
		{
			input_vals[k] = std::pow(static_cast<float>(k) / 255.0f, 2.2f);
		}
		return Texpainter::Snap{input_vals};
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
		m_handle = GTK_DRAWING_AREA(widget);
		m_img_surface = nullptr;
		r_eh = nullptr;
		gtk_widget_add_events(widget,
		                      GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK
		                         | GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK | GDK_SCROLL_MASK);
		g_signal_connect(G_OBJECT(widget), "draw", G_CALLBACK(draw_callback), this);
		g_signal_connect(G_OBJECT(widget), "button-press-event", G_CALLBACK(on_mouse_down), this);
		g_signal_connect(G_OBJECT(widget), "button-release-event", G_CALLBACK(on_mouse_up), this);
		g_signal_connect(G_OBJECT(widget), "motion-notify-event", G_CALLBACK(on_mouse_move), this);
		g_signal_connect(G_OBJECT(widget), "key-press-event", G_CALLBACK(on_key_press), this);
		g_signal_connect(G_OBJECT(widget), "key-release-event", G_CALLBACK(on_key_release), this);
		gtk_widget_set_can_focus(widget, TRUE);
	}

	~Impl()
	{
		if(m_img_surface != nullptr) { cairo_surface_destroy(m_img_surface); }
		gtk_widget_destroy(GTK_WIDGET(m_handle));
		g_object_unref(m_handle);
		m_impl = nullptr;
	}

	void render(Geom::Dimension dim, cairo_t* cr)
	{
		//		auto context = gtk_widget_get_style_context(GTK_WIDGET(m_handle));
		//		gtk_render_background(context, cr, 0, 0, dim.width(), dim.height());
		cairo_set_source_rgba(cr, 1.0, 0.0, 1.0, 1.0);
		cairo_rectangle(cr, 0.0, 0.0, m_size_current.width(), m_size_current.height());
		if(m_img_surface != nullptr) [[likely]]
			{
				cairo_set_source_surface(cr, m_img_surface, 0.0, 0.0);
				cairo_rectangle(cr, 0.0, 0.0, m_size_current.width(), m_size_current.height());
				cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
				cairo_fill(cr);
			}
	}

	void image(Model::Image const& img)
	{
		if(img.size() != m_size_current || m_img_surface == nullptr) [[unlikely]]
			{
				auto surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, img.width(), img.height());
				if(cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) { return; }

				if(surface == nullptr) { abort(); }

				cairo_surface_destroy(m_img_surface);
				m_img_surface = surface;
				m_size_current = img.size();
			}
		update(img);
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

	Size2d imageSize() const noexcept
	{
		return m_size_current;
	}

	void focus()
	{
		gtk_widget_grab_focus(GTK_WIDGET(m_handle));
	}

private:
	void* r_eh;
	EventHandlerVtable m_vt;

	cairo_surface_t* m_img_surface;
	Size2d m_size_current;

	void update(Model::Image const& img)
	{
		auto const stride = cairo_image_surface_get_stride(m_img_surface);
		cairo_surface_flush(m_img_surface);
		auto const data = cairo_image_surface_get_data(m_img_surface);
		assert(data != nullptr);
		auto const w = img.width();
		auto const h = img.height();
		auto read_ptr = std::data(img.pixels());
		for(uint32_t row = 0; row < h; ++row)
		{
			auto write_ptr = data + row * stride;
			for(uint32_t col = 0; col < w; ++col)
			{
				auto pixel_out = read_ptr->value();

				write_ptr[0] = gamma_22.nearestIndex(pixel_out[2]);
				write_ptr[1] = gamma_22.nearestIndex(pixel_out[1]);
				write_ptr[2] = gamma_22.nearestIndex(pixel_out[0]);
				write_ptr[3] = gamma_22.nearestIndex(pixel_out[3]);

				write_ptr += 4;
				++read_ptr;
			}
		}
		cairo_surface_mark_dirty(m_img_surface);
		gtk_widget_queue_draw(GTK_WIDGET(m_handle));
	}


	GtkDrawingArea* m_handle; // TODO: Should be a gl area
	static gboolean draw_callback(GtkWidget* widget, cairo_t* cr, gpointer self)
	{
		auto w = gtk_widget_get_allocated_width(widget);
		auto h = gtk_widget_get_allocated_height(widget);
		reinterpret_cast<Impl*>(self)->render(Geom::Dimension{}.width(w).height(h), cr);
		return FALSE;
	}

	static gboolean on_mouse_down(GtkWidget* w, GdkEvent* e, gpointer self)
	{
		auto& obj = *reinterpret_cast<Impl*>(self);
		if(obj.r_eh != nullptr)
		{
			if(!gtk_widget_is_focus(w))
			{
				gtk_widget_grab_focus(w);
				return FALSE;
			}
			auto event_button = reinterpret_cast<GdkEventButton const*>(e);
			obj.m_vt.m_on_mouse_down(obj.r_eh,
			                         obj,
			                         vec2_t{event_button->x, event_button->y},
			                         vec2_t{event_button->x_root, event_button->y_root},
			                         event_button->button);
			return FALSE;
		}
		return TRUE;
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
			return FALSE;
		}
		return TRUE;
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

	static gboolean on_key_press(GtkWidget* widget, GdkEvent* event, gpointer self)
	{
		auto obj = reinterpret_cast<Impl*>(self);
		auto& key = event->key;
		auto scancode = key.hardware_keycode - 8;
#ifndef __linux__
#waring "Scancode key offset is not tested. Pressing esc should print 1"
//	printf("%d\n", key.hardware_keycode - 8);
#endif
		if(obj->r_eh != nullptr) { obj->m_vt.on_key_down(obj->r_eh, *obj, scancode); }
		return TRUE;
	}

	static gboolean on_key_release(GtkWidget* widget, GdkEvent* event, gpointer self)
	{
		auto obj = reinterpret_cast<Impl*>(self);
		auto& key = event->key;
		auto scancode = key.hardware_keycode - 8;
#ifndef __linux__
#waring "Scancode key offset is not tested. Pressing esc should print 1"
		printf("%d\n", key.hardware_keycode - 8);
#endif
		if(obj->r_eh != nullptr) { obj->m_vt.on_key_up(obj->r_eh, *obj, scancode); }
		return TRUE;
	}
};

Texpainter::Ui::ImageView::ImageView(Container& cnt): m_impl{new Impl{cnt}}
{
}

Texpainter::Ui::ImageView::~ImageView()
{
	delete m_impl;
}

Texpainter::Ui::ImageView& Texpainter::Ui::ImageView::image(Model::Image const& img)
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