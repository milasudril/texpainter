//@	{"targets":[{"name":"brush_selector.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_BRUSHSELECTOR_HPP
#define TEXPAINTER_APP_BRUSHSELECTOR_HPP

#include "ui/combobox.hpp"
#include "ui/slider.hpp"
#include "ui/box.hpp"

namespace Texpainter::App
{
	class BrushSelector
	{
	public:
		explicit BrushSelector(Ui::Container& owner)
		    : m_root{owner, Ui::Box::Orientation::Horizontal}
		    , m_shape{m_root}
		    , m_size{m_root.insertMode(Ui::Box::InsertMode{0, Ui::Box::Fill | Ui::Box::Expand}),
		             false}
		{
		}

	private:
		Ui::Box m_root;
		Ui::Combobox m_shape;
		Ui::Slider m_size;
	};
}

#endif