//@	{"targets":[{"name":"image_surface.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./image_surface.hpp"

#include <gtk/gtk.h>

class Texpainter::Ui::ImageSurface::Impl: private ImageSurface
{
	public:
		explicit Impl(Container& cnt):ImageSurface{*this}
		{
			auto widget = gtk_drawing_area_new();
			g_object_ref_sink(widget);
			cnt.add(widget);
			m_handle = GTK_DRAWING_AREA(widget);
			g_signal_connect(G_OBJECT(widget), "draw", G_CALLBACK(draw_callback), this);
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
			gtk_render_background (context, cr, 0, 0, dim.width(), dim.height());

			cairo_arc (cr,
						dim.width() / 2.0, dim.height() / 2.0,
						std::min(dim.width(), dim.height()) / 2.0,
						0, 2 * G_PI);

			GdkRGBA color;
			gtk_style_context_get_color (context,
										gtk_style_context_get_state (context),
										&color);
			gdk_cairo_set_source_rgba (cr, &color);
			cairo_fill (cr);
		}

	private:
		Callback r_cb;
		void* r_cb_obj;
		GtkDrawingArea* m_handle;  // TODO: Should be a gl area
		static gboolean draw_callback(GtkWidget* widget, cairo_t* cr, gpointer* self)
		{
			auto w = gtk_widget_get_allocated_width(widget);
			auto h = gtk_widget_get_allocated_height(widget);
			reinterpret_cast<Impl*>(self)->render(Geom::Dimension{}.width(w).height(h), cr);
			return FALSE;
		}
};

Texpainter::Ui::ImageSurface::ImageSurface(Container& cnt):
m_impl{new Impl{cnt}}
{}

Texpainter::Ui::ImageSurface::~ImageSurface()
{ delete m_impl; }