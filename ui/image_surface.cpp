//@	{"targets":[{"name":"image_surface.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./image_surface.hpp"
#include "model/image.hpp"

#include <gtk/gtk.h>

namespace
{
	bool realloc_surface_needed(Texpainter::Model::Image const& new_image, Texpainter::Model::Image const* old_image)
	{
		if(old_image == nullptr)
		{ return true;}

		return size(new_image) != size(*old_image);
	}
}

class Texpainter::Ui::ImageSurface::Impl: private ImageSurface
{
	public:
		explicit Impl(Container& cnt):ImageSurface{*this}
		{
			auto widget = gtk_drawing_area_new();
			g_object_ref_sink(widget);
			cnt.add(widget);
			m_handle = GTK_DRAWING_AREA(widget);
			r_img = nullptr;
			m_img_surface = nullptr;
			r_eh = nullptr;
			gtk_widget_add_events(widget, GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_KEY_PRESS_MASK | GDK_SCROLL_MASK);
			g_signal_connect(G_OBJECT(widget), "draw", G_CALLBACK(draw_callback), this);
			g_signal_connect(G_OBJECT(widget), "button-press-event", G_CALLBACK(on_mouse_down), this);
		}

		~Impl()
		{
			if( m_img_surface != nullptr)
			{ cairo_surface_destroy(m_img_surface); }
			gtk_widget_destroy(GTK_WIDGET(m_handle));
			g_object_unref(m_handle);
			m_impl = nullptr;
		}

		void render(Geom::Dimension dim, cairo_t* cr)
		{
			auto context = gtk_widget_get_style_context(GTK_WIDGET(m_handle));
			gtk_render_background (context, cr, 0, 0, dim.width(), dim.height());
			if(m_img_surface != nullptr)
			{
				cairo_set_source_surface(cr, m_img_surface, 0.0, 0.0);
				auto const w = r_img->width();
				auto const h = r_img->height();
				cairo_rectangle(cr, 0.0, 0.0, w, h);
				cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
				cairo_fill(cr);
			}
		}

		void image(Model::Image const& img)
		{
			if(realloc_surface_needed(img, r_img))
			{
				auto surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, img.width(), img.height());
				if( cairo_surface_status( surface) != CAIRO_STATUS_SUCCESS)
				{ return; }

				if( m_img_surface != nullptr)
				{ cairo_surface_destroy(m_img_surface); }
				m_img_surface = surface;
			}
			r_img = &img;
			update();
		}

		void update()
		{
			auto const stride = cairo_image_surface_get_stride(m_img_surface);
			cairo_surface_flush(m_img_surface);
			auto const data = cairo_image_surface_get_data(m_img_surface);
			auto const w = r_img->width();
			auto const h = r_img->height();
			auto read_ptr = r_img->pixels();
			for(uint32_t row = 0; row < h; ++row)
			{
				auto write_ptr = data + row*stride;
				for(uint32_t col = 0; col < w; ++col)
				{
					auto pixel_out = *read_ptr/255.0f;
					write_ptr[0] = pixel_out.blue();
					write_ptr[1] = pixel_out.green();
					write_ptr[2] = pixel_out.red();
					write_ptr[3] = pixel_out.alpha();
					write_ptr+=4;
					++read_ptr;
				}
			}
			cairo_surface_finish(m_img_surface);
		}

		void eventHandler(void* event_handler, EventHandlerVtable const& vtable)
		{
			r_eh = event_handler;
			m_vt = vtable;
		}

	private:
		void* r_eh;
		EventHandlerVtable m_vt;

		cairo_surface_t* m_img_surface;
		Model::Image const* r_img;

		GtkDrawingArea* m_handle;  // TODO: Should be a gl area
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
				auto event_button = reinterpret_cast<GdkEventButton const*>(e);
				obj.m_vt.m_on_mouse_down(obj.r_eh,
				          obj,
				          vec2_t{event_button->x, event_button->y},
				          vec2_t{event_button->x_root, event_button->y_root});
				return FALSE;
			}
			return TRUE;
		}
};

Texpainter::Ui::ImageSurface::ImageSurface(Container& cnt):
m_impl{new Impl{cnt}}
{}

Texpainter::Ui::ImageSurface::~ImageSurface()
{ delete m_impl; }

Texpainter::Ui::ImageSurface& Texpainter::Ui::ImageSurface::image(Model::Image const& img)
{
	m_impl->image(img);
	return *this;
}

Texpainter::Ui::ImageSurface& Texpainter::Ui::ImageSurface::eventHandler(void* event_handler, EventHandlerVtable const& vtable)
{
	m_impl->eventHandler(event_handler, vtable);
	return *this;
}

