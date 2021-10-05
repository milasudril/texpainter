//@	{
//@	 "targets":[{"name":"glarea.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]
//@	}

#include "glarea.hpp"
#include "container.hpp"
#include <gtk/gtk.h>

class Texpainter::Ui::GLArea::Impl final: private GLArea
{
public:
	explicit Impl(Container& cnt);
	~Impl();

	void minHeight(int h) noexcept { gtk_widget_set_size_request(GTK_WIDGET(m_handle), -1, h); }

	void minWidth(int w) { gtk_widget_set_size_request(GTK_WIDGET(m_handle), w, -1); }

	void minSize(int w, int h) { gtk_widget_set_size_request(GTK_WIDGET(m_handle), w, h); }

	void eventHandler(void* cb_obj, const EventHandlerVtable& vt)
	{
		r_cb_obj = cb_obj;
		m_vt     = vt;
	}

	void glActivate() { gtk_gl_area_make_current(m_handle); }

	void versionRequest(int major, int minor)
	{
		gtk_gl_area_set_required_version(m_handle, major, minor);
	}

	void redraw() noexcept { gtk_gl_area_queue_render(m_handle); }

	void activate() noexcept { gtk_gl_area_make_current(m_handle); }

private:
	void* r_cb_obj;
	EventHandlerVtable m_vt;
	GtkGLArea* m_handle;

	static gboolean render(GtkGLArea*, GdkGLContext*, void* user_data)
	{
		auto self = reinterpret_cast<Impl*>(user_data);
		if(self->r_cb_obj != nullptr) { self->m_vt.render(self->r_cb_obj, *self); }
		return TRUE;
	}

	static gboolean resize(GtkGLArea*, int width, int height, void* user_data)
	{
		auto self = reinterpret_cast<Impl*>(user_data);
		if(self->r_cb_obj != nullptr) { self->m_vt.resize(self->r_cb_obj, *self, width, height); }
		return TRUE;
	}

	static void realize(GtkWidget*, void* user_data)
	{
		auto self = reinterpret_cast<Impl*>(user_data);
		gtk_gl_area_make_current(self->m_handle);
		if(self->r_cb_obj != nullptr) { self->m_vt.realize(self->r_cb_obj, *self); }
	}
};

Texpainter::Ui::GLArea::Impl::Impl(Container& cnt): GLArea(*this)
{
	r_cb_obj     = nullptr;
	auto gl_area = GTK_GL_AREA(gtk_gl_area_new());
	gtk_gl_area_set_required_version(gl_area, 4, 6);
	g_signal_connect(gl_area, "render", G_CALLBACK(render), this);
	g_signal_connect(gl_area, "resize", G_CALLBACK(resize), this);
	g_signal_connect(gl_area, "realize", G_CALLBACK(realize), this);
	gtk_gl_area_set_has_depth_buffer(gl_area, TRUE);
	gtk_gl_area_set_has_stencil_buffer(gl_area, TRUE);
	gtk_gl_area_set_auto_render(gl_area, FALSE);
	cnt.add(gl_area);

	m_handle = gl_area;
}

Texpainter::Ui::GLArea::Impl::~Impl()
{
	m_impl   = nullptr;
	r_cb_obj = nullptr;
	gtk_widget_destroy(GTK_WIDGET(m_handle));
}

Texpainter::Ui::GLArea::GLArea(Container& cnt): m_impl(new Impl(cnt)) {}

Texpainter::Ui::GLArea::~GLArea() { delete m_impl; }

Texpainter::Ui::GLArea& Texpainter::Ui::GLArea::minHeight(int h)
{
	m_impl->minHeight(h);
	return *this;
}

Texpainter::Ui::GLArea& Texpainter::Ui::GLArea::minWidth(int w)
{
	m_impl->minWidth(w);
	return *this;
}

Texpainter::Ui::GLArea& Texpainter::Ui::GLArea::minSize(int w, int h)
{
	m_impl->minSize(w, h);
	return *this;
}

Texpainter::Ui::GLArea& Texpainter::Ui::GLArea::glActivate()
{
	m_impl->glActivate();
	return *this;
}

Texpainter::Ui::GLArea& Texpainter::Ui::GLArea::versionRequest(int major, int minor)
{
	m_impl->versionRequest(major, minor);
	return *this;
}
Texpainter::Ui::GLArea& Texpainter::Ui::GLArea::eventHandler(void* cb_obj,
                                                             const EventHandlerVtable& vt)
{
	m_impl->eventHandler(cb_obj, vt);
	return *this;
}

Texpainter::Ui::GLArea& Texpainter::Ui::GLArea::redraw() noexcept
{
	m_impl->redraw();
	return *this;
}

Texpainter::Ui::GLArea& Texpainter::Ui::GLArea::activate() noexcept
{
	m_impl->activate();
	return *this;
}