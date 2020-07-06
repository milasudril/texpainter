//@	{"targets":[{"name":"color_picker.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./color_picker.hpp"

#include "./box.hpp"
#include "./image_view.hpp"
#include "./slider.hpp"
#include "./labeled_input.hpp"

#include "model/hsi_rgb.hpp"

#include <gtk/gtk.h>
#include <cassert>

namespace
{
	Texpainter::Model::Image gen_colors(float intensity, float alpha, Texpainter::Size2d size)
	{
		Texpainter::Model::Image ret{size};
		generate(ret.pixels(), [intensity, alpha, size](auto col, auto row) {
			auto x = static_cast<float>(col) / size.width();
			auto y = 1.0f - static_cast<float>(row) / size.height();
			auto ret = toRgb(Texpainter::Model::Hsi{x, y, intensity, alpha});
			return ret;
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
		    row <= std::min(static_cast<int>(end_coords[1]), static_cast<int>(h) - 1);
		    ++row)
		{
			for(auto col = std::max(static_cast<int>(begin_coords[0]), 0);
			    col <= std::min(static_cast<int>(end_coords[0]), static_cast<int>(w) - 1);
			    ++col)
			{
				auto const loc_ret = Texpainter::vec2_t{static_cast<double>(col), static_cast<double>(row)};
				auto d = loc_ret - pos;
				if(Texpainter::dot(d, d) < radius * radius)
				{
					auto& pixel = canvas(col, row);
					auto const i = intensity(pixel);
					pixel = i >= 1.0f ? Texpainter::Model::Pixel{0.0f, 0.0f, 0.0f, 1.0f} :
					                    Texpainter::Model::Pixel{1.0f, 1.0f, 1.0f, 1.0f};
					pixel.alpha(1.0f);
				}
			}
		}
	}

	constexpr auto gen_tickmarks()
	{
		constexpr char const* tick_mark_text[] = {"  -14",
		                                          "",
		                                          "  -12",
		                                          "",
		                                          "  -10",
		                                          "",
		                                          "   -8",
		                                          "",
		                                          "   -6",
		                                          "",
		                                          "   -4",
		                                          "",
		                                          "   -2",
		                                          "",
		                                          "    0",
		                                          ""};

		constexpr double values[] = {std::exp2(-14.0f) / 3.0f,
		                             std::exp2(-13.0f) / 3.0f,
		                             std::exp2(-12.0f) / 3.0f,
		                             std::exp2(-11.0f) / 3.0f,
		                             std::exp2(-10.0f) / 3.0f,
		                             std::exp2(-9.0f) / 3.0f,
		                             std::exp2(-8.0f) / 3.0f,
		                             std::exp2(-7.0f) / 3.0f,
		                             std::exp2(-6.0f) / 3.0f,
		                             std::exp2(-5.0f) / 3.0f,
		                             std::exp2(-4.0f) / 3.0f,
		                             std::exp2(-3.0f) / 3.0f,
		                             std::exp2(-2.0f) / 3.0f,
		                             std::exp2(-1.0f) / 3.0f,
		                             std::exp2(0.0f) / 3.0f,
		                             std::exp2(1.0f) / 3.0f};
		std::array<Texpainter::Ui::Slider::TickMark, std::size(tick_mark_text)> ret{};

		for(size_t k = 0; k < std::size(ret); ++k)
		{
			ret[k] = {Texpainter::Ui::logValue(values[k], -16), tick_mark_text[k]};
		}

		return ret;
	}

	constexpr auto intensity_tickmarks = gen_tickmarks();
}

class Texpainter::Ui::ColorPicker::Impl: private Texpainter::Ui::ColorPicker
{
public:
	enum class ControlId : int
	{
		Colors,
		Intensity,
		Alpha
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
		m_intensity.inputField().value(logValue(m_hsi.intensity, -16));
		m_alpha.inputField().value(linValue(m_hsi.alpha));
		m_colors_cache = gen_colors(m_hsi.intensity, m_hsi.alpha, Size2d{384, 384});
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
	Box m_cols;
	LabeledInput<ImageView> m_colors;
	LabeledInput<Slider> m_intensity;
	LabeledInput<Slider> m_alpha;

	void update()
	{
		auto colors = m_colors_cache;
		draw_marker(vec2_t{384.0, 384.0} * vec2_t{m_hsi.hue, 1.0 - m_hsi.saturation}, colors.pixels());
		m_colors.inputField().image(colors);
	}
};

template<>
void Texpainter::Ui::ColorPicker::ColorPicker::Impl::onMouseDown<
   Texpainter::Ui::ColorPicker::ColorPicker::Impl::ControlId::Colors>(ImageView&,
                                                                      vec2_t pos_window,
                                                                      vec2_t,
                                                                      int button)
{
	m_btn_state |= (1 << button);
	if(m_btn_state == 2)
	{
		auto pos = pos_window / vec2_t{384.0, 384.0};
		m_hsi.hue = std::clamp(pos[0], 0.0, 1.0);
		m_hsi.saturation = 1.0f - std::clamp(pos[1], 0.0, 1.0);
		update();
	}
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
                                                                      vec2_t)
{
	if(m_btn_state == 2)
	{
		auto pos = pos_window / vec2_t{384.0, 384.0};
		m_hsi.hue = std::clamp(pos[0], 0.0, 1.0);
		m_hsi.saturation = 1.0f - std::clamp(pos[1], 0.0, 1.0);
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
	m_colors_cache = gen_colors(m_hsi.intensity, m_hsi.alpha, Size2d{384, 384});
	update();
}

template<>
void Texpainter::Ui::ColorPicker::Impl::onChanged<
   Texpainter::Ui::ColorPicker::Impl::ControlId::Alpha>(Slider& src)
{
	m_hsi.alpha = static_cast<float>(linValue(src.value()));
	m_colors_cache = gen_colors(m_hsi.intensity, m_hsi.alpha, Size2d{384, 384});
	update();
}

Texpainter::Ui::ColorPicker::Impl::Impl(Container& cnt):
   Texpainter::Ui::ColorPicker{*this},
   m_colors_cache{Size2d{384, 384}},
   m_root{cnt, Box::Orientation::Vertical},
   m_cols{m_root, Box::Orientation::Horizontal},
   m_colors{m_cols, Box::Orientation::Vertical, "Saturation \\ Hue"},
   m_intensity{m_cols, Box::Orientation::Vertical, "I/evFS", true},
   m_alpha{m_root, Box::Orientation::Horizontal, "Opacity: ", false}
{
	m_btn_state = 0;
	std::ranges::fill(m_colors_cache.pixels(), Model::Pixel{0.0f, 0.0f, 0.0f, 1.0f});
	m_colors.inputField()
	   .minSize(Size2d{384, 384})
	   .eventHandler<ControlId::Colors>(*this)
	   .alwaysEmitMouseEvents(true);
	value(Model::Pixel{0.5f, 0.5f, 0.5f, 1.0f});
	m_intensity.inputField().eventHandler<ControlId::Intensity>(*this).ticks(intensity_tickmarks);
	m_alpha.inputField().eventHandler<ControlId::Alpha>(*this);
}

Texpainter::Ui::ColorPicker::Impl::~Impl()
{
	m_impl = nullptr;
}

Texpainter::Ui::ColorPicker const& Texpainter::Ui::ColorPicker::self() const noexcept
{
	return m_impl->self();
}
