//@	{
//@  "targets":[{"name":"color_picker_sidepanel.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UI_COLORPICKERSIDEPANEL_HPP
#define TEXPAINTER_UI_COLORPICKERSIDEPANEL_HPP

#include "./box.hpp"
#include "./combobox.hpp"
#include "./labeled_input.hpp"
#include "./text_entry.hpp"
#include "./color_picker.hpp"

#include <optional>
#include <string>

namespace Texpainter::Ui
{
	class ColorPickerSidepanel
	{
	private:
		using NormalizeFunc = float (*)(Model::Pixel);

		class Normalizer
		{
		public:
			explicit Normalizer(float value, NormalizeFunc f): m_value{value}, r_f{f} {}

			auto operator()(Model::Pixel pixel) const
			{
				auto alpha_tmp = pixel.alpha();
				auto x_new     = m_value * pixel / r_f(pixel);
				x_new.alpha(alpha_tmp);
				return x_new;
			}

		private:
			float m_value;
			NormalizeFunc r_f;
		};

	public:
		enum class ItemId : int
		{
			NormalizeIntensityMode,
			NormalizeIntensityValue,
		};

		ColorPickerSidepanel(ColorPickerSidepanel const&&) = delete;
		ColorPickerSidepanel& operator=(ColorPickerSidepanel const&&) = delete;

		explicit ColorPickerSidepanel(Container& container, ColorPicker& picker)
		    : r_picker{picker}
		    , m_box{container, Box::Orientation::Vertical}
		    , m_normalize_intensity{m_box, Box::Orientation::Vertical, "No intensity normalization"}
		{
			m_normalization_function = [](Model::Pixel) { return 1.0f; };
			m_normalize_intensity.inputField()
			    .content("0.5")
			    .enabled(false)
			    .eventHandler<ItemId::NormalizeIntensityValue>(*this);
			m_normalize_intensity.label()
			    .append("Normalize to max(RGB)")
			    .append("Normalize to mean(RGB)")
			    .selected(0)
			    .eventHandler<ItemId::NormalizeIntensityMode>(*this);
		}

		template<ItemId id>
		void onChanged(Combobox& box);

		template<ItemId id>
		void onChanged(TextEntry& entry);

		auto targetIntensity() const
		{
			return Normalizer{
			    m_normalize_intensity.label().selected() == 0
			        ? 1.0f
			        : static_cast<float>(std::atof(m_normalize_intensity.inputField().content())),
			    m_normalization_function};
		}

	private:
		ColorPicker& r_picker;
		Box m_box;
		LabeledInput<TextEntry, Combobox> m_normalize_intensity;
		NormalizeFunc m_normalization_function;
	};

	template<>
	inline void ColorPickerSidepanel::onChanged<
	    ColorPickerSidepanel::ItemId::NormalizeIntensityValue>(TextEntry& entry)
	{
		auto value            = static_cast<float>(std::atof(entry.content()));
		auto color            = r_picker.value();
		auto color_normalized = Normalizer{value, m_normalization_function}(color);
		auto max_val          = max(color_normalized);
		if(max_val > 1.0f)
		{
			color_normalized /= max_val;
			value    = m_normalization_function(color_normalized);
			auto str = std::to_string(value);
			entry.content(str.c_str());
		}
	}

	template<>
	inline void ColorPickerSidepanel::onChanged<
	    ColorPickerSidepanel::ItemId::NormalizeIntensityMode>(Combobox& box)
	{
		switch(box.selected())
		{
			case 0:
				m_normalize_intensity.inputField().enabled(false);
				m_normalization_function = [](Model::Pixel x) { return 1.0f; };
				break;

			case 1:
				m_normalize_intensity.inputField().enabled(true).focus();
				m_normalization_function = [](Model::Pixel x) { return max(x); };
				onChanged<ColorPickerSidepanel::ItemId::NormalizeIntensityValue>(
				    m_normalize_intensity.inputField());
				break;

			case 2:
				m_normalize_intensity.inputField().enabled(true).focus();
				m_normalization_function = [](Model::Pixel x) { return intensity(x); };

				onChanged<ColorPickerSidepanel::ItemId::NormalizeIntensityValue>(
				    m_normalize_intensity.inputField());
				break;
		}
	}


}

#endif