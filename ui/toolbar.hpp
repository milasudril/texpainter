//@	{
//@  "targets":[{"name":"toolbar.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UI_TOOLBAR_HPP
#define TEXPAINTER_UI_TOOLBAR_HPP

#include "./box.hpp"

namespace Texpainter::Ui
{
	template<auto EnumType, template<decltype(EnumType)> class EnumTypeTraits>
	class Toolbar
	{
	public:
		using ControlId = decltype(EnumType);


	private:
		Box m_box;
		EnumTuple<ControlId, EnumTypeTraits> m_widgetes;
	};
}

#endif