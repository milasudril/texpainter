//@	{"targets":[{"name":"color_picker.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./color_picker.hpp"

#include "./box.hpp"
#include "./image_view.hpp"
#include "./slider.hpp"

#include "model/hsi_rgb.hpp"

#include <gtk/gtk.h>

class Texpainter::Ui::ColorPicker::Impl: private Texpainter::Ui::ColorPicker
{
public:
	enum class ControlId : int
	{
		Colors,
		Intensity
	};

	Impl(Container& cnt);
	~Impl();

	Model::Pixel value() const
	{
		return m_value;
	}

	void value(Model::Pixel val)
	{
		m_value = val;
	}

	ColorPicker const& self() const noexcept
	{
		return *this;
	}

	template<ControlId>
	void onChanged(Slider&);

private:
	Box m_root;
	ImageView m_colors;
	Slider m_intensity;

	Model::Pixel m_value;
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

namespace
{
	Texpainter::Model::Image genColors(float intensity, float alpha, Texpainter::Size2d size)
	{
		Texpainter::Model::Image ret{size};
		generate(ret.pixels(), [intensity, alpha, size](auto col, auto row) {
			auto x = static_cast<float>(col) / size.width();
			auto y = 1.0f - static_cast<float>(row) / size.height();
			return convert(Texpainter::Model::Hsi{
			   Texpainter::Angle{x, Texpainter::Angle::Turns{}}, y, intensity, alpha});
		});
		return ret;
	}
}

template<>
void Texpainter::Ui::ColorPicker::Impl::onChanged<
   Texpainter::Ui::ColorPicker::Impl::ControlId::Intensity>(Slider& src)
{
	auto colors = genColors(logValue(src.value(), -20), 1.0f, Size2d{384, 384});
	m_colors.minSize(Size2d{384, 384}).image(colors);
}

Texpainter::Ui::ColorPicker::Impl::Impl(Container& cnt):
   Texpainter::Ui::ColorPicker{*this},
   m_root{cnt, Box::Orientation::Horizontal},
   m_colors{m_root},
   m_intensity{m_root, true},
   m_value{0.5f, 0.5f, 0.5f, 1.0f}
{
	auto colors = genColors(0.5, 1.0f, Size2d{384, 384});
	m_colors.minSize(Size2d{384, 384}).image(colors);
	m_intensity.eventHandler<ControlId::Intensity>(*this);
}

Texpainter::Ui::ColorPicker::Impl::~Impl()
{
	m_impl = nullptr;
}

Texpainter::Ui::ColorPicker const& Texpainter::Ui::ColorPicker::self() const noexcept
{
	return m_impl->self();
}
