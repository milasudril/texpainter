//@	{
//@  "targets":[{"name":"color_picker_sidepanel.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UI_COLORPICKERSIDEPANEL_HPP
#define TEXPAINTER_UI_COLORPICKERSIDEPANEL_HPP

#include "./box.hpp"
#include "./button.hpp"
#include "./labeled_input.hpp"
#include "./text_entry.hpp"

#include <optional>

namespace Texpainter::Ui
{
	class ColorPickerSidepanel
	{
	public:
		enum class ItemId : int
		{
			NormailzeIntensityEnable
		};
		ColorPickerSidepanel(ColorPickerSidepanel const&&) = delete;
		ColorPickerSidepanel& operator=(ColorPickerSidepanel const&&) = delete;

		explicit ColorPickerSidepanel(Container& container):
		   m_box{container, Box::Orientation::Vertical},
		   m_normalize_intensity{m_box, Box::Orientation::Vertical, "Normalize intensity to"}
		{
			m_normalize_intensity.inputField().content("0.5").enabled(false);
			m_normalize_intensity.label().state(false).eventHandler<ItemId::NormailzeIntensityEnable>(*this);
		}

		template<ItemId id>
		void onClicked(Button& btn);

		float targetIntensity() const
		{
			if(m_normalize_intensity.label().state())
			{ return std::atof(m_normalize_intensity.inputField().content()); }
			else
			{
				return -1.0f;
			}
		}

	private:
		Box m_box;
		LabeledInput<TextEntry, Button> m_normalize_intensity;
	};


	template<>
	void ColorPickerSidepanel::onClicked<ColorPickerSidepanel::ItemId::NormailzeIntensityEnable>(
	   Button& btn)
	{
		if(btn.state()) { m_normalize_intensity.inputField().enabled(true).focus(); }
		else
		{
			m_normalize_intensity.inputField().enabled(false);
		}
	}
}

#endif