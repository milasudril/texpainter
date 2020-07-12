//@	{
//@	 "targets":[{"name":"submenu.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UI_SUBMENU_HPP
#define TEXPAINTER_UI_SUBMENU_HPP

#include "./container.hpp"
#include "./menu_item.hpp"
#include "./menu.hpp"

#include <utility>

namespace Texpainter::Ui
{
	class Submenu
	{
	public:
		explicit Submenu(Container& parent, char const* label = ""):
		   m_menuitem{parent, label},
		   m_menu{m_menuitem}
		{
		}

		Submenu& label(char const* lab)
		{
			m_menuitem.label(lab);
			return *this;
		}

	private:
		MenuItem m_menuitem;
		Menu m_menu;
	};
}

#endif
