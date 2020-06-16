//@	{
//@	 "targets":[{"name":"extended_color_picker.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_EXTENDEDCOLORPICKER_HPP
#define TEXPAINTER_EXTENDEDCOLORPICKER_HPP

#include "./container.hpp"
#include "./color_picker.hpp"
#include "./box.hpp"

#include <utility>

namespace Texpainter::Ui
{
	template<class Sidepanel>
	class ExtendedColorPicker
	{
	public:
		template<class... Args>
		explicit ExtendedColorPicker(Container& container, Args&&... args):
		   m_outer{container, Box::Orientation::Horizontal},
		   m_picker{m_outer},
		   m_sidepanel{m_outer, std::forward<Args>(args)...}
		{
		}

		Sidepanel const& sidepanel() const
		{
			return m_sidepanel;
		}

		Sidepanel& sidepanel()
		{
			return m_sidepanel;
		}

		auto value() const
		{
			return m_picker.value();
		}

		ExtendedColorPicker& value(Model::Pixel val)
		{
			m_picker.value(val);
			return *this;
		}

	private:
		Box m_outer;
		ColorPicker m_picker;
		Sidepanel m_sidepanel;
	};
}

#endif