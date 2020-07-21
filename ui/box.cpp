//@	{
//@  "targets":[{"name":"box.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]
//@	}

#include "box.hpp"

#include <gtk/gtk.h>

class Texpainter::Ui::Box::Impl final: private Box
{
public:
	Impl(Container& cnt, Orientation orientation, int global_spacing);
	~Impl();

	void _add(GtkWidget* handle) noexcept
	{
		if(m_mode.flags & PositionBack)
		{
			gtk_box_pack_end(
			    m_handle, handle, m_mode.flags & Expand, m_mode.flags & Fill, m_mode.padding);
		}
		else
		{
			gtk_box_pack_start(
			    m_handle, handle, m_mode.flags & Expand, m_mode.flags & Fill, m_mode.padding);
		}
	}

	void _show() noexcept { gtk_widget_show_all(GTK_WIDGET(m_handle)); }

	void _sensitive(bool val) { gtk_widget_set_sensitive(GTK_WIDGET(m_handle), val); }

	void* _toplevel() const { return gtk_widget_get_toplevel(GTK_WIDGET(m_handle)); }

	void homogenous(bool status) noexcept { gtk_box_set_homogeneous(m_handle, status); }


	void insertMode(const InsertMode& mode) noexcept { m_mode = mode; }

	void alignment(float x) noexcept;

	Orientation orientation() const noexcept { return m_orientation; }

private:
	static void destroy_callback(GtkWidget* object, gpointer user_data);
	GtkBox* m_handle;
	InsertMode m_mode;
	Orientation m_orientation;
};

Texpainter::Ui::Box::Box(Container& cnt, Orientation orientation, int global_spacing)
{
	m_impl = new Texpainter::Ui::Box::Impl(cnt, orientation, global_spacing);
}

Texpainter::Ui::Box::~Box() { delete m_impl; }

Texpainter::Ui::Box& Texpainter::Ui::Box::add(void* handle)
{
	m_impl->_add(GTK_WIDGET(handle));
	return *this;
}

Texpainter::Ui::Box& Texpainter::Ui::Box::show()
{
	m_impl->_show();
	return *this;
}

Texpainter::Ui::Box& Texpainter::Ui::Box::sensitive(bool val)
{
	m_impl->_sensitive(val);
	return *this;
}

void* Texpainter::Ui::Box::toplevel() const { return m_impl->_toplevel(); }

Texpainter::Ui::Box& Texpainter::Ui::Box::homogenous(bool status) noexcept
{
	m_impl->homogenous(status);
	return *this;
}

Texpainter::Ui::Box& Texpainter::Ui::Box::insertMode(const InsertMode& mode) noexcept
{
	m_impl->insertMode(mode);
	return *this;
}

Texpainter::Ui::Box& Texpainter::Ui::Box::alignment(float x) noexcept
{
	m_impl->alignment(x);
	return *this;
}

Texpainter::Ui::Box::Orientation Texpainter::Ui::Box::orientation() const noexcept
{
	return m_impl->orientation();
}

Texpainter::Ui::Box::Impl::Impl(Container& cnt, Orientation orientation, int global_spacing)
    : Box(*this)
    , m_mode{0, 0}
{
	m_orientation = orientation;
	auto widget   = gtk_box_new(orientation == Orientation::Vertical ? GTK_ORIENTATION_VERTICAL
                                                                   : GTK_ORIENTATION_HORIZONTAL,
                              global_spacing);
	g_object_ref_sink(widget);
	cnt.add(widget);
	m_handle = GTK_BOX(widget);
}

Texpainter::Ui::Box::Impl::~Impl()
{
	m_impl = nullptr;
	gtk_widget_destroy(GTK_WIDGET(m_handle));
	g_object_unref(m_handle);
}

void Texpainter::Ui::Box::Impl::alignment(float x) noexcept
{
	if(x < 1.0f / 3.0f) { gtk_box_set_baseline_position(m_handle, GTK_BASELINE_POSITION_TOP); }
	else if(x > 2.0f / 3.0f)
	{
		gtk_box_set_baseline_position(m_handle, GTK_BASELINE_POSITION_BOTTOM);
	}
	else
	{
		{
			gtk_box_set_baseline_position(m_handle, GTK_BASELINE_POSITION_CENTER);
		}
	}
}