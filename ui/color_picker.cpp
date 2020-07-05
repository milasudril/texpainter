//@	{"targets":[{"name":"color_picker.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./color_picker.hpp"

#include "./box.hpp"
#include "./image_view.hpp"
#include "./slider.hpp"

#include "model/hsi_rgb.hpp"

#include <gtk/gtk.h>

namespace
{
	Texpainter::Model::Image gen_colors(float intensity, float alpha, Texpainter::Size2d size)
	{
		Texpainter::Model::Image ret{size};
		generate(ret.pixels(), [intensity, alpha, size](auto col, auto row) {
			auto x = static_cast<float>(col) / size.width();
			auto y = 1.0f - static_cast<float>(row) / size.height();
			return toRgb(Texpainter::Model::Hsi{x, y, intensity, alpha});
		});
		return ret;
	}

	void draw_marker(Texpainter::vec2_t pos, Texpainter::Span2d<Texpainter::Model::Pixel> canvas)
	{
		constexpr auto radius = 2;
		auto const w = canvas.width();
		auto const h = canvas.height();
		auto const begin_coords = pos - Texpainter::vec2_t{radius, radius};
		auto const end_coords = pos + Texpainter::vec2_t{radius, radius};

		for(auto row = std::max(static_cast<int>(begin_coords[1]), 0);
		    row < std::min(static_cast<int>(end_coords[1]), static_cast<int>(h));
		    ++row)
		{
			for(auto col = std::max(static_cast<int>(begin_coords[0]), 0);
			    col < std::min(static_cast<int>(end_coords[0]), static_cast<int>(w));
			    ++col)
			{
				auto const loc_ret = Texpainter::vec2_t{static_cast<double>(col), static_cast<double>(row)};
				auto d = loc_ret - pos;
				if(Texpainter::dot(d, d) < radius * radius)
				{
					auto& pixel = canvas(col, row);
					pixel = Texpainter::Model::Pixel{1.0f, 1.0f, 1.0f, 1.0f} - pixel;
					pixel.alpha(1.0f);
				}
			}
		}
	}
}

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
		return toRgb(m_hsi);
	}

	void value(Model::Pixel val)
	{
		m_hsi = toHsi(val);
		m_intensity.value(logValue(m_hsi.intensity, -16));
		m_colors_cache = gen_colors(m_hsi.intensity, 1.0f, Size2d{384, 384});
		update();
	}

	ColorPicker const& self() const noexcept
	{
		return *this;
	}

	template<ControlId>
	void onChanged(Slider&);

	template<ControlId>
	void onKeyDown(ImageView&, int)
	{
	}

	template<ControlId>
	void onKeyUp(ImageView&, int)
	{
	}

	template<ControlId>
	void onMouseDown(ImageView& src, vec2_t pos_window, vec2_t pos_screen, int button);

	template<ControlId>
	void onMouseUp(ImageView& src, vec2_t pos_window, vec2_t pos_screen, int button);

	template<ControlId>
	void onMouseMove(ImageView& src, vec2_t pos_window, vec2_t pos_screen);

private:
	Model::Hsi m_hsi;
	Model::Image m_colors_cache;
	uint32_t m_btn_state;

	Box m_root;
	ImageView m_colors;
	Slider m_intensity;

	void update()
	{
		auto colors = m_colors_cache;
		draw_marker(vec2_t{384.0, 384.0} * vec2_t{m_hsi.hue, 1.0 - m_hsi.saturation}, colors.pixels());
		m_colors.image(colors);
	}
};

template<>
void Texpainter::Ui::ColorPicker::ColorPicker::Impl::onMouseDown<
   Texpainter::Ui::ColorPicker::ColorPicker::Impl::ControlId::Colors>(ImageView&,
                                                                      vec2_t,
                                                                      vec2_t,
                                                                      int button)
{
	m_btn_state |= (1 << button);
}

template<>
void Texpainter::Ui::ColorPicker::ColorPicker::Impl::onMouseUp<
   Texpainter::Ui::ColorPicker::ColorPicker::Impl::ControlId::Colors>(ImageView&,
                                                                      vec2_t,
                                                                      vec2_t,
                                                                      int button)
{
	m_btn_state &= (~(1 << button));
}

template<>
void Texpainter::Ui::ColorPicker::ColorPicker::Impl::onMouseMove<
   Texpainter::Ui::ColorPicker::ColorPicker::Impl::ControlId::Colors>(ImageView& view,
                                                                      vec2_t pos_window,
                                                                      vec2_t pos_screen)
{
	if(m_btn_state == 2)
	{
		auto pos = pos_window / vec2_t{384.0, 384.0};
		m_hsi.hue = pos[0];
		m_hsi.saturation = 1.0f - pos[1];
		update();
	}
}

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

template<>
void Texpainter::Ui::ColorPicker::Impl::onChanged<
   Texpainter::Ui::ColorPicker::Impl::ControlId::Intensity>(Slider& src)
{
	m_hsi.intensity = logValue(src.value(), -16);
	m_colors_cache = gen_colors(m_hsi.intensity, 1.0f, Size2d{384, 384});
	update();
}

Texpainter::Ui::ColorPicker::Impl::Impl(Container& cnt):
   Texpainter::Ui::ColorPicker{*this},
   m_colors_cache{Size2d{384, 384}},
   m_root{cnt, Box::Orientation::Horizontal},
   m_colors{m_root},
   m_intensity{m_root, true}
{
	m_btn_state = 0;
	m_colors.minSize(Size2d{384, 384})
	   .eventHandler<ControlId::Colors>(*this)
	   .alwaysEmitMouseEvents(true);
	value(Model::Pixel{0.5f, 0.5f, 0.5f, 1.0f});
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
