//@	{
//@  "targets":[{"name":"color_picker_sidepanel.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UI_COLORPICKERSIDEPANEL_HPP
#define TEXPAINTER_UI_COLORPICKERSIDEPANEL_HPP

#include "./box.hpp"
#include "./combobox.hpp"
#include "./labeled_input.hpp"
#include "./text_entry.hpp"

#include <optional>

namespace Texpainter::Ui
{
	class ColorPickerSidepanel
	{
	private:
		using NormalizeFunc = Model::Pixel (*)(Model::Pixel, float target_intensity);

		class Normalizer
		{
		public:
			explicit Normalizer(float value, NormalizeFunc f): m_value{value}, r_f{f}
			{}

			auto operator()(Model::Pixel pixel) const
			{
				return r_f(pixel, m_value);
			}

		private:
			float m_value;
			NormalizeFunc r_f;
		};

	public:
		enum class ItemId : int
		{
			NormailzeIntensityEnable
		};

		ColorPickerSidepanel(ColorPickerSidepanel const&&) = delete;
		ColorPickerSidepanel& operator=(ColorPickerSidepanel const&&) = delete;

		explicit ColorPickerSidepanel(Container& container):
		   m_box{container, Box::Orientation::Vertical},
		   m_normalize_intensity{m_box, Box::Orientation::Vertical, "No intensity normalization"}
		{
			m_normalize_intensity.inputField().content("0.5").enabled(false);
			m_normalize_intensity.label()
				.append("Normalize to max(RGB)")
				.append("Normalize to mean(RGB)")
				.selected(0).eventHandler<ItemId::NormailzeIntensityEnable>(*this);
			m_normalization_function = [](Model::Pixel x, float){ return x;};
		}

		template<ItemId id>
		void onChanged(Combobox& box);

		auto targetIntensity() const
		{
			return Normalizer{static_cast<float>(std::atof(m_normalize_intensity.inputField().content())), m_normalization_function};
		}

	private:
		Box m_box;
		LabeledInput<TextEntry, Combobox> m_normalize_intensity;
		NormalizeFunc m_normalization_function;
	};


	template<>
	void ColorPickerSidepanel::onChanged<ColorPickerSidepanel::ItemId::NormailzeIntensityEnable>(
	   Combobox& box)
	{
		switch(box.selected())
		{
			case 0:
				m_normalize_intensity.inputField().enabled(false);
				m_normalization_function = [](Model::Pixel x, float){ return x;};
				break;

			case 1:
				m_normalize_intensity.inputField().enabled(true).focus();
				m_normalization_function = [](Model::Pixel x, float target_intensity){
					auto alpha_tmp = x.alpha();
					x = target_intensity*x/max(x);
					x.alpha(alpha_tmp);
					return x;
				};
				break;

			case 2:
				m_normalize_intensity.inputField().enabled(true).focus();
				m_normalization_function = [](Model::Pixel x, float target_intensity){
					auto alpha_tmp = x.alpha();
					x = target_intensity*x/intensity(x);
					x.alpha(alpha_tmp);
					return x;
				};
				break;
		}
	}
}

#endif