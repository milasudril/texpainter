//@	{"targets":[{"name":"palette_creator.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_PALETTECREATOR_HPP
#define TEXPAINTER_APP_PALETTECREATOR_HPP

#include "./size_input.hpp"

#include "model/item_name.hpp"
#include "model/palette_generate.hpp"
#include "pixel_store/palette.hpp"
#include "pixel_store/hsi_rgb.hpp"
#include "ui/box.hpp"
#include "ui/labeled_input.hpp"
#include "ui/text_entry.hpp"
#include "ui/slider.hpp"
#include "ui/palette_view.hpp"

namespace Texpainter
{
	namespace PaletteCreator_detail
	{
		constexpr auto genTicks()
		{
			std::array<Ui::Slider::TickMark, 12> ret;
			for(size_t k = 0; k < ret.size(); ++k)
			{
				ret[k] = Ui::Slider::TickMark{
				    Ui::SliderValue{static_cast<double>(k) / static_cast<double>(ret.size())}, ""};
			}
			return ret;
		}
	}

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
			BaseHue,
			Deco1,
			Deco2,
			Deco3,
			Saturation
		};

		struct PaletteInfo
		{
			Model::ItemName name;
			Model::PaletteParameters params;
		};

		explicit PaletteCreator(Ui::Container& container)
		    : m_root{container, Ui::Box::Orientation::Vertical}
		    , m_name{m_root, Ui::Box::Orientation::Horizontal, "Name: "}
		    , m_base_hue{m_root, Ui::Box::Orientation::Horizontal, "Base hue: "}
		    , m_deco_1{m_root, Ui::Box::Orientation::Horizontal, "Deco 1: "}
		    , m_deco_2{m_root, Ui::Box::Orientation::Horizontal, "Deco 2: "}
		    , m_deco_3{m_root, Ui::Box::Orientation::Horizontal, "Deco 3: "}
		    , m_saturation{m_root, Ui::Box::Orientation::Horizontal, "Saturation: ", false}
		    , m_intensity_map{m_root, Ui::Box::Orientation::Horizontal}
		    , m_intensity_map_label{m_intensity_map, "Map intensity: "}
		    , m_to_intensity{m_intensity_map, "To intensity"}
		    , m_to_alpha{m_intensity_map, "To alpha"}
		    , m_order{m_root, Ui::Box::Orientation::Horizontal}
		    , m_order_label{m_order, "Order: "}
		    , m_by_intensity{m_order, "By intensity"}
		    , m_reversed{m_order, "Reversed"}
		{
			constexpr auto ticks = PaletteCreator_detail::genTicks();
			m_base_hue.inputField().slider().eventHandler<ControlId::BaseHue>(*this).ticks(ticks);
			m_deco_1.inputField()
			    .slider()
			    .eventHandler<ControlId::Deco1>(*this)
			    .value(Ui::SliderValue{0.5})
			    .ticks(ticks);
			m_deco_2.inputField()
			    .slider()
			    .eventHandler<ControlId::Deco2>(*this)
			    .value(Ui::SliderValue{0.5})
			    .ticks(ticks);
			m_deco_3.inputField()
			    .slider()
			    .eventHandler<ControlId::Deco3>(*this)
			    .value(Ui::SliderValue{0.5})
			    .ticks(ticks);
			m_saturation.inputField().eventHandler<ControlId::Saturation>(*this).value(
			    Ui::SliderValue{1.0});
			m_to_alpha.state(true);
			m_to_intensity.state(true);

			updateSliders();
		}

		template<ControlId>
		void onChanged(Ui::Slider&)
		{
			updateSliders();
		}

		PaletteInfo value() const
		{
			return PaletteInfo{Model::ItemName{m_name.inputField().content()},
			                   generateColors(),
			                   m_to_intensity.state(),
			                   m_to_alpha.state(),
			                   m_by_intensity.state(),
			                   m_reversed.state()};
		}

	private:
		Ui::Box m_root;
		Ui::LabeledInput<Ui::TextEntry> m_name;
		Ui::LabeledInput<SliderWithPalView> m_base_hue;
		Ui::LabeledInput<SliderWithPalView> m_deco_1;
		Ui::LabeledInput<SliderWithPalView> m_deco_2;
		Ui::LabeledInput<SliderWithPalView> m_deco_3;
		Ui::LabeledInput<Ui::Slider> m_saturation;
		Ui::Box m_intensity_map;
		Ui::Label m_intensity_map_label;
		Ui::Button m_to_intensity;
		Ui::Button m_to_alpha;
		Ui::Box m_order;
		Ui::Label m_order_label;
		Ui::Button m_by_intensity;
		Ui::Button m_reversed;

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
