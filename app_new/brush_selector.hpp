//@	{"targets":[{"name":"brush_selector.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_BRUSHSELECTOR_HPP
#define TEXPAINTER_APP_BRUSHSELECTOR_HPP

#include "ui/combobox.hpp"

namespace Texpainter::App
{
	class BrushSelector
	{
	public:
		explicit BrushSelector(Ui::Container& owner): m_selector{owner} {}

	private:
		Ui::Combobox m_selector;
	};
}

#endif