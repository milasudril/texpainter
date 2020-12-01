//@	{"targets":[{"name":"palette_selector.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_PALETTESELECTOR_HPP
#define TEXPAINTER_APP_PALETTESELECTOR_HPP

#include "ui/combobox.hpp"
#include "ui/palette_view.hpp"

namespace Texpainter::App
{
	class PaletteSelector
	{
	public:
		explicit PaletteSelector(Ui::Container& owner)
		    : m_root{owner, Ui::Box::Orientation::Horizontal}
		    , m_name{m_root}
		    , m_palette{m_root.insertMode(Ui::Box::InsertMode{0, Ui::Box::Fill | Ui::Box::Expand})}
		{
		}

		template<auto id, class EventHandler>
		PaletteSelector& eventHandler(EventHandler& eh)
		{
			m_name.eventHandler<id>(eh);
			m_palette.eventHandler<id>(eh);
			return *this;
		}

	private:
		Ui::Box m_root;
		Ui::Combobox m_name;
		Ui::PaletteView m_palette;
	};
}

#endif