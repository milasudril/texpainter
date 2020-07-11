//@	{"targets":[{"name":"color_picker.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./color_picker.hpp"

#include "./box.hpp"
#include "./image_view.hpp"
#include "./slider.hpp"
#include "./labeled_input.hpp"
#include "./text_entry.hpp"
#include "./separator.hpp"
#include "./button.hpp"
#include "./palette_view.hpp"

#include "model/hsi_rgb.hpp"
#include "utils/numconv.hpp"

#include <gtk/gtk.h>

#include <cassert>
#include <random>

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

	enum class ControlId : int
	{
		Colors,
		Intensity,
		Alpha,
		Red,
		Green,
		Blue,
		Hex,
		Hue,
		Saturation,
		Random,
		PredefColors
	};
}

class Texpainter::Ui::ColorPicker::Impl: private Texpainter::Ui::ColorPicker
{
public:
	Impl(Container& cnt,
	     PolymorphicRng rng,
	     char const* predef_label,
	     std::span<Model::Pixel const> predef_colors);
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
	void onKeyDown(ImageView&, int val);

	template<ControlId>
	void onKeyUp(ImageView&, int);

	template<ControlId>
	void onMouseDown(ImageView& src, vec2_t pos_window, vec2_t pos_screen, int button);

	template<ControlId>
	void onMouseUp(ImageView& src, vec2_t pos_window, vec2_t pos_screen, int button);

	template<ControlId>
	void onMouseMove(ImageView& src, vec2_t pos_window, vec2_t pos_screen);

	template<ControlId>
	void onClicked(Button& btn);


	template<ControlId>
	void onMouseDown(Texpainter::Ui::PaletteView& view, size_t, int)
	{
	}

	template<ControlId>
	void onMouseUp(Texpainter::Ui::PaletteView&, size_t, int);

	template<ControlId>
	void onMouseMove(Texpainter::Ui::PaletteView&, size_t)
	{
	}

	template<ControlId>
	void onChanged(Texpainter::Ui::TextEntry&);

private:
	Model::Hsi m_hsi;
	Model::Image m_colors_cache;
	PolymorphicRng m_rng;

	uint32_t m_btn_state;
	int m_key;

	Box m_root;
	Box m_cols;
	Box m_left;
	Box m_leftcols;
	LabeledInput<ImageView> m_colors;
	LabeledInput<Slider> m_intensity;
	LabeledInput<Slider> m_alpha;
	Separator m_lrsep;
	Box m_right;
	Box m_rgb_inner;
	LabeledInput<TextEntry> m_red;
	LabeledInput<TextEntry> m_green;
	LabeledInput<TextEntry> m_blue;
	LabeledInput<TextEntry> m_hex;
	Separator m_num_sep_1;
	Box m_hsi_inner;
	LabeledInput<TextEntry> m_hue;
	LabeledInput<TextEntry> m_saturation;
	LabeledInput<TextEntry> m_intensity_text;
	Separator m_num_sep_2;
	LabeledInput<TextEntry> m_alpha_text;
	Separator m_num_sep_3;
	Button m_random;
	Separator m_num_sep_4;
	Box m_bottom_box;
	LabeledInput<PaletteView> m_predef_colors;
	LabeledInput<PaletteView> m_current_color;

	void update()
	{
		auto colors = m_colors_cache;
		draw_marker(vec2_t{384.0, 384.0} * vec2_t{m_hsi.hue, 1.0 - m_hsi.saturation}, colors.pixels());
		m_colors.inputField().image(colors);
		auto const rgb = toRgb(m_hsi);

		m_red.inputField().content(toArray(rgb.red()).data());
		m_green.inputField().content(toArray(rgb.green()).data());
		m_blue.inputField().content(toArray(rgb.blue()).data());

		if(rgb.red() > 1.0f || rgb.green() > 1.0f || rgb.blue() > 1.0f) [[unlikely]]
			{
				m_hex.inputField().content("------");
			}
		else
		{
			auto const rgb_g22 = Model::BasicPixel<Model::ColorProfiles::Gamma22>{rgb};
			auto const val = 255.0f * rgb_g22.value();
			std::array<char, 8> hex_str{};
			sprintf(hex_str.data(),
			        "%02X%02X%02X",
			        static_cast<uint8_t>(val[0]),
			        static_cast<uint8_t>(val[1]),
			        static_cast<uint8_t>(val[2]));
			m_hex.inputField().content(hex_str.data());
		}

		m_hue.inputField().content(toArray(m_hsi.hue).data());
		m_saturation.inputField().content(toArray(m_hsi.saturation).data());
		m_intensity_text.inputField().content(toArray(m_hsi.intensity).data());
		m_alpha_text.inputField().content(toArray(m_hsi.alpha).data());

		Model::Palette pal{1};
		pal[0] = rgb;
		m_current_color.inputField().palette(pal).update();
	}

	void updateHueSaturation(vec2_t pos_window)
	{
		auto pos = pos_window / vec2_t{384.0, 384.0};
		switch(m_key)
		{
			case 29: m_hsi.hue = std::clamp(pos[0], 0.0, 1.0); break;

			case 42: m_hsi.saturation = 1.0f - std::clamp(pos[1], 0.0, 1.0); break;

			default:
				m_hsi.hue = std::clamp(pos[0], 0.0, 1.0);
				m_hsi.saturation = 1.0f - std::clamp(pos[1], 0.0, 1.0);
		}
		update();
	}
};

Texpainter::Ui::ColorPicker::ColorPicker(Container& cnt,
                                         PolymorphicRng rng,
                                         char const* predef_label,
                                         std::span<Model::Pixel const> predef_colors)
{
	m_impl = new Impl(cnt, rng, predef_label, predef_colors);
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
void Texpainter::Ui::ColorPicker::ColorPicker::Impl::onMouseDown<ControlId::Colors>(
   ImageView&, vec2_t pos_window, vec2_t, int button)
{
	m_btn_state |= (1 << button);
	if(m_btn_state == 2) { updateHueSaturation(pos_window); }
}

template<>
void Texpainter::Ui::ColorPicker::ColorPicker::Impl::onMouseUp<ControlId::Colors>(ImageView&,
                                                                                  vec2_t,
                                                                                  vec2_t,
                                                                                  int button)
{
	m_btn_state &= (~(1 << button));
}

template<>
void Texpainter::Ui::ColorPicker::ColorPicker::Impl::onMouseMove<ControlId::Colors>(
   ImageView& view, vec2_t pos_window, vec2_t)
{
	if(m_btn_state == 2) { updateHueSaturation(pos_window); }
}

template<>
void Texpainter::Ui::ColorPicker::ColorPicker::Impl::onKeyDown<ControlId::Colors>(ImageView&,
                                                                                  int val)
{
	m_key = val;
}

template<>
void Texpainter::Ui::ColorPicker::ColorPicker::Impl::onKeyUp<ControlId::Colors>(ImageView&, int)
{
	m_key = 0;
}

template<>
void Texpainter::Ui::ColorPicker::Impl::onChanged<ControlId::Intensity>(Slider& src)
{
	m_hsi.intensity = logValue(src.value(), -16);
	m_colors_cache = gen_colors(m_hsi.intensity, m_hsi.alpha, Size2d{384, 384});
	update();
}

template<>
void Texpainter::Ui::ColorPicker::Impl::onChanged<ControlId::Alpha>(Slider& src)
{
	m_hsi.alpha = static_cast<float>(linValue(src.value()));
	m_colors_cache = gen_colors(m_hsi.intensity, m_hsi.alpha, Size2d{384, 384});
	update();
}

template<>
void Texpainter::Ui::ColorPicker::ColorPicker::Impl::onClicked<ControlId::Random>(Button& btn)
{
	std::uniform_real_distribution U{0.0f, 1.0f};
	btn.state(false);
	value(Model::Pixel{U(m_rng), U(m_rng), U(m_rng), U(m_rng)});
}

template<>
void Texpainter::Ui::ColorPicker::ColorPicker::Impl::onMouseUp<ControlId::PredefColors>(
   Texpainter::Ui::PaletteView& src, size_t index, int button)
{
	if(button == 1) { value(src.color(index)); }
}

template<>
void Texpainter::Ui::ColorPicker::ColorPicker::Impl::onChanged<ControlId::Red>(
   Texpainter::Ui::TextEntry& src)
{
	auto const val = toFloat(src.content());
	if(!val.has_value())
	{
		src.content(toArray(toRgb(m_hsi).red()).data());
		return;
	}
	value(toRgb(m_hsi).red(std::clamp(*val, 0.0f, 1.0f)));
}

template<>
void Texpainter::Ui::ColorPicker::ColorPicker::Impl::onChanged<ControlId::Green>(
   Texpainter::Ui::TextEntry& src)
{
	auto const val = toFloat(src.content());
	if(!val.has_value())
	{
		src.content(toArray(toRgb(m_hsi).green()).data());
		return;
	}
	value(toRgb(m_hsi).green(std::clamp(*val, 0.0f, 1.0f)));
}

template<>
void Texpainter::Ui::ColorPicker::ColorPicker::Impl::onChanged<ControlId::Blue>(
   Texpainter::Ui::TextEntry& src)
{
	auto const val = toFloat(src.content());
	if(!val.has_value())
	{
		src.content(toArray(toRgb(m_hsi).blue()).data());
		return;
	}
	value(toRgb(m_hsi).blue(std::clamp(*val, 0.0f, 1.0f)));
}

template<>
void Texpainter::Ui::ColorPicker::ColorPicker::Impl::onChanged<ControlId::Hex>(
   Texpainter::Ui::TextEntry& src)
{
}

template<>
void Texpainter::Ui::ColorPicker::ColorPicker::Impl::onChanged<ControlId::Hue>(
   Texpainter::Ui::TextEntry& src)
{
	auto const val = toFloat(src.content());
	if(!val.has_value())
	{
		src.content(toArray(m_hsi.hue).data());
		return;
	}
	m_hsi.hue = std::clamp(*val, 0.0f, 1.0f);
	update();
}

template<>
void Texpainter::Ui::ColorPicker::ColorPicker::Impl::onChanged<ControlId::Saturation>(
   Texpainter::Ui::TextEntry& src)
{
	auto const val = toFloat(src.content());
	if(!val.has_value())
	{
		src.content(toArray(m_hsi.saturation).data());
		return;
	}
	m_hsi.saturation = std::clamp(*val, 0.0f, 1.0f);
	update();
}

template<>
void Texpainter::Ui::ColorPicker::ColorPicker::Impl::onChanged<ControlId::Intensity>(
   Texpainter::Ui::TextEntry& src)
{
	auto const val = toFloat(src.content());
	if(!val.has_value())
	{
		src.content(toArray(m_hsi.intensity).data());
		return;
	}
	m_hsi.intensity = std::clamp(*val, 0.0f, 1.0f);
	m_intensity.inputField().value(logValue(m_hsi.intensity, -16));
	m_colors_cache = gen_colors(m_hsi.intensity, m_hsi.alpha, Size2d{384, 384});
	update();
}

template<>
void Texpainter::Ui::ColorPicker::ColorPicker::Impl::onChanged<ControlId::Alpha>(
   Texpainter::Ui::TextEntry& src)
{
	auto const val = toFloat(src.content());
	if(!val.has_value())
	{
		src.content(toArray(m_hsi.alpha).data());
		return;
	}
	m_hsi.alpha = std::clamp(*val, 0.0f, 1.0f);
	m_alpha.inputField().value(linValue(m_hsi.alpha));
	update();
}

Texpainter::Ui::ColorPicker::Impl::Impl(Container& cnt,
                                        PolymorphicRng rng,
                                        char const* predef_label,
                                        std::span<Model::Pixel const> predef_colors):
   Texpainter::Ui::ColorPicker{*this},
   m_colors_cache{Size2d{384, 384}},
   m_rng{rng},
   m_root{cnt, Box::Orientation::Vertical},
   m_cols{m_root, Box::Orientation::Horizontal},
   m_left{m_cols, Box::Orientation::Vertical},
   m_leftcols{m_left, Box::Orientation::Horizontal},
   m_colors{m_leftcols, Box::Orientation::Vertical, "Saturation \\ Hue"},
   m_intensity{m_leftcols, Box::Orientation::Vertical, "I/evFS", true},
   m_alpha{m_left, Box::Orientation::Horizontal, "Opacity: ", false},
   m_lrsep{m_cols},
   m_right{m_cols, Box::Orientation::Vertical},
   m_rgb_inner{m_right, Box::Orientation::Vertical},
   m_red{m_rgb_inner, Box::Orientation::Horizontal, "R: "},
   m_green{m_rgb_inner, Box::Orientation::Horizontal, "G: "},
   m_blue{m_rgb_inner, Box::Orientation::Horizontal, "B: "},
   m_hex{m_rgb_inner, Box::Orientation::Horizontal, "Hex: "},
   m_num_sep_1{m_right},
   m_hsi_inner{m_right, Box::Orientation::Vertical},
   m_hue{m_hsi_inner, Box::Orientation::Horizontal, "H: "},
   m_saturation{m_hsi_inner, Box::Orientation::Horizontal, "S: "},
   m_intensity_text{m_hsi_inner, Box::Orientation::Horizontal, "I: "},
   m_num_sep_2{m_right},
   m_alpha_text{m_right, Box::Orientation::Horizontal, "α: "},
   m_num_sep_3{m_right},
   m_random{m_right, "Random"},
   m_num_sep_4{m_right},
   m_bottom_box{m_root, Box::Orientation::Horizontal},
   m_predef_colors{m_bottom_box.insertMode(Ui::Box::InsertMode{0, Ui::Box::Fill | Ui::Box::Expand}),
                   Box::Orientation::Vertical,
                   predef_label},
   m_current_color{m_bottom_box.insertMode(Ui::Box::InsertMode{0, 0}),
                   Box::Orientation::Vertical,
                   "Selected color:"}
{
	m_btn_state = 0;
	m_key = 0;
	std::ranges::fill(m_colors_cache.pixels(), Model::Pixel{0.0f, 0.0f, 0.0f, 1.0f});
	m_colors.inputField()
	   .minSize(Size2d{384, 384})
	   .eventHandler<ControlId::Colors>(*this)
	   .alwaysEmitMouseEvents(true);
	value(Model::Pixel{0.5f, 0.5f, 0.5f, 1.0f});
	m_intensity.inputField().eventHandler<ControlId::Intensity>(*this).ticks(intensity_tickmarks);
	m_alpha.inputField().eventHandler<ControlId::Alpha>(*this);

	m_red.inputField().small(true).width(13).eventHandler<ControlId::Red>(*this);
	m_green.inputField().small(true).width(13).eventHandler<ControlId::Green>(*this);
	m_blue.inputField().small(true).width(13).eventHandler<ControlId::Blue>(*this);
	m_hex.inputField().small(true).width(8).eventHandler<ControlId::Hex>(*this);

	m_hue.inputField().small(true).width(13).eventHandler<ControlId::Hue>(*this);
	m_saturation.inputField().small(true).width(13).eventHandler<ControlId::Saturation>(*this);
	m_intensity_text.inputField().small(true).width(13).eventHandler<ControlId::Intensity>(*this);

	m_alpha_text.inputField().small(true).width(13).eventHandler<ControlId::Alpha>(*this);

	m_random.small(true).eventHandler<ControlId::Random>(*this);

	m_predef_colors.inputField().palette(predef_colors).eventHandler<ControlId::PredefColors>(*this);
	Model::Palette pal{1};
	pal[0] = toRgb(m_hsi);
	m_current_color.inputField().palette(pal).minSize(Size2d{48, 48}).update();
}

Texpainter::Ui::ColorPicker::Impl::~Impl()
{
	m_impl = nullptr;
}

Texpainter::Ui::ColorPicker const& Texpainter::Ui::ColorPicker::self() const noexcept
{
	return m_impl->self();
}
