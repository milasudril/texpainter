//@	{"targets":[{"name":"palette_selector.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_PALETTESELECTOR_HPP
#define TEXPAINTER_APP_PALETTESELECTOR_HPP

#include "ui/combobox.hpp"

namespace Texpainter::App
{
	class PaletteSelector
	{
	public:
		explicit PaletteSelector(Ui::Container& owner): m_selector{owner} {}

	private:
		Ui::Combobox m_selector;
	};
}

#endif