//@	{"targets":[{"name":"color_picker.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./color_picker.hpp"

#include <gtk/gtk.h>

class Texpainter::Ui::ColorPicker::Impl: private Texpainter::Ui::ColorPicker
{
public:
	Impl(Container& cnt);
	~Impl();

	Model::Pixel value() const
	{
		GdkRGBA tmp;
		gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(m_handle), &tmp);
		return toLinear(Model::BasicPixel<Model::ColorProfiles::Gamma22>{static_cast<float>(tmp.red),
		                                                                 static_cast<float>(tmp.green),
		                                                                 static_cast<float>(tmp.blue),
		                                                                 static_cast<float>(tmp.alpha)});
	}

	void value(Model::Pixel val)
	{
		auto const g22 = Model::BasicPixel<Model::ColorProfiles::Gamma22>{val};
		GdkRGBA tmp{g22.red(), g22.green(), g22.blue(), g22.alpha()};
		gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(m_handle), &tmp);
	}

	ColorPicker const& self() const noexcept
	{
		return *this;
	}

private:
	GtkColorChooserWidget* m_handle;
};

Texpainter::Ui::ColorPicker::ColorPicker(Container& cnt)
{
	m_impl = new Impl(cnt);
}

Texpainter::Ui::ColorPicker::~ColorPicker()
{
	delete m_impl;
}

Texpainter::Model::Pixel Texpainter::Ui::ColorPicker::value() const noexcept
{
	return m_impl->value();
}

Texpainter::Ui::ColorPicker& Texpainter::Ui::ColorPicker::value(Model::Pixel color)
{
	m_impl->value(color);
	return *this;
}


Texpainter::Ui::ColorPicker::Impl::Impl(Container& cnt): Texpainter::Ui::ColorPicker{*this}
{
	auto widget = gtk_color_chooser_widget_new();
	GValue show_editor = G_VALUE_INIT;
	g_value_init(&show_editor, G_TYPE_BOOLEAN);
	g_value_set_boolean(&show_editor, TRUE);
	g_object_set_property(G_OBJECT(widget), "show-editor", &show_editor);
	g_object_ref_sink(widget);
	cnt.add(widget);
	g_value_reset(&show_editor);
	m_handle = GTK_COLOR_CHOOSER_WIDGET(widget);
}

Texpainter::Ui::ColorPicker::Impl::~Impl()
{
	m_impl = nullptr;
	gtk_widget_destroy(GTK_WIDGET(m_handle));
	g_object_unref(m_handle);
}

Texpainter::Ui::ColorPicker const& Texpainter::Ui::ColorPicker::self() const noexcept
{
	return m_impl->self();
}
