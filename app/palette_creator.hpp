//@	{"targets":[{"name":"palette_creator.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_PALETTECREATOR_HPP
#define TEXPAINTER_APP_PALETTECREATOR_HPP

#include "./size_input.hpp"

#include "pixel_store/palette.hpp"
#include "pixel_store/hsi_rgb.hpp"
#include "ui/box.hpp"
#include "ui/labeled_input.hpp"
#include "ui/text_entry.hpp"
#include "ui/slider.hpp"
#include "ui/palette_view.hpp"

namespace Texpainter
{
	class PaletteCreator
	{
		class SliderWithPalView
		{
		public:
			SliderWithPalView(Ui::Container& container)
			    : m_root{container, Ui::Box::Orientation::Horizontal}
			    , m_slider{m_root.insertMode(
			                   Ui::Box::InsertMode{0, Ui::Box::Fill | Ui::Box::Expand}),
			               false}
			    , m_color{m_root.insertMode(Ui::Box::InsertMode{0, 0})}
			{
				m_color.minSize(Size2d{32, 32});
			}

			Ui::Slider& slider() { return m_slider; }

			Ui::Slider const& slider() const { return m_slider; }

			Ui::PaletteView& color() { return m_color; }

		private:
			Ui::Box m_root;
			Ui::Slider m_slider;
			Ui::PaletteView m_color;
		};

	public:
		enum class ControlId : int
		{
			Name,
			BaseHue,
			Deco1,
			Deco2,
			Deco3,
			Saturation
		};

		struct PaletteInfo
		{
			std::string name;
			std::array<PixelStore::Pixel, 4> colors;
		};

		explicit PaletteCreator(Ui::Container& container)
		    : m_root{container, Ui::Box::Orientation::Vertical}
		    , m_name{m_root, Ui::Box::Orientation::Horizontal, "Name: "}
		    , m_base_hue{m_root, Ui::Box::Orientation::Horizontal, "Base hue: "}
		    , m_deco_1{m_root, Ui::Box::Orientation::Horizontal, "Deco 1: "}
		    , m_deco_2{m_root, Ui::Box::Orientation::Horizontal, "Deco 2: "}
		    , m_deco_3{m_root, Ui::Box::Orientation::Horizontal, "Deco 3: "}
		    , m_saturation{m_root, Ui::Box::Orientation::Horizontal, "Saturation: ", false}
		{
			m_base_hue.inputField().slider().eventHandler<ControlId::BaseHue>(*this);
			m_deco_1.inputField().slider().eventHandler<ControlId::Deco1>(*this).value(
			    Ui::SliderValue{0.5});
			m_deco_2.inputField().slider().eventHandler<ControlId::Deco2>(*this).value(
			    Ui::SliderValue{0.5});
			m_deco_3.inputField().slider().eventHandler<ControlId::Deco3>(*this).value(
			    Ui::SliderValue{0.5});
			m_saturation.inputField().eventHandler<ControlId::Saturation>(*this).value(
			    Ui::SliderValue{1.0});

			updateSliders();
		}

		template<ControlId>
		void onChanged(Ui::Slider&)
		{
			updateSliders();
		}

		PaletteInfo value() const
		{
			return PaletteInfo{m_name.inputField().content(), generateColors()};
		}

	private:
		Ui::Box m_root;
		Ui::LabeledInput<Ui::TextEntry> m_name;
		Ui::LabeledInput<SliderWithPalView> m_base_hue;
		Ui::LabeledInput<SliderWithPalView> m_deco_1;
		Ui::LabeledInput<SliderWithPalView> m_deco_2;
		Ui::LabeledInput<SliderWithPalView> m_deco_3;
		Ui::LabeledInput<Ui::Slider> m_saturation;

		std::array<PixelStore::Pixel, 4> generateColors() const
		{
			auto const base_hue =
			    static_cast<float>(linValue(m_base_hue.inputField().slider().value()));

			vec4_t hue;
			hue[0] = 0.0f;
			hue[1] =
			    static_cast<float>(linValue(m_deco_1.inputField().slider().value(), -0.5, 0.5));
			hue[2] =
			    static_cast<float>(linValue(m_deco_2.inputField().slider().value(), -0.5, 0.5));
			hue[3] =
			    static_cast<float>(linValue(m_deco_3.inputField().slider().value(), -0.5, 0.5));

			hue += base_hue;
			hue = hue < 0.0f ? hue + 1.0f : hue;
			hue = hue >= 1.0f ? hue - 1.0f : hue;

			auto const saturation = static_cast<float>(linValue(m_saturation.inputField().value()));

			std::array<PixelStore::Pixel, 4> colors;
			std::ranges::for_each(colors, [saturation, hue, k = 0](auto& value) mutable {
				auto constexpr intensity = 1.0f / 3.0f;
				auto constexpr alpha     = 1.0f;
				value = toRgb(PixelStore::Hsi{hue[k], saturation, intensity, alpha});
				++k;
			});

			return colors;
		}

		void updateSliders()
		{
			auto colors = generateColors();
			m_base_hue.inputField().color().palette({&colors[0], 1}).update();
			m_deco_1.inputField().color().palette({&colors[1], 1}).update();
			m_deco_2.inputField().color().palette({&colors[2], 1}).update();
			m_deco_3.inputField().color().palette({&colors[3], 1}).update();
		}
	};
}

#endif
