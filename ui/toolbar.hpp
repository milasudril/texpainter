//@	{
//@  "targets":[{"name":"toolbar.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UI_TOOLBAR_HPP
#define TEXPAINTER_UI_TOOLBAR_HPP

#include "./box.hpp"
#include "utils/empty.hpp"

#include "./button.hpp"

namespace Texpainter::Ui
{
	template<class EnumType, template<EnumType> class EnumTypeTraits>
	class Toolbar
	{
	public:
		using ControlId = EnumType;

		explicit Toolbar(Container& owner, Box::Orientation orientation):
		   m_box{owner, orientation},
		   m_widgets{m_box, ""}
		{
		}

		template<EnumType item>
		auto const& get() const
		{
			return Texpainter::get<item>(m_widgets);
		}

		template<EnumType item>
		auto& get()
		{
			return Texpainter::get<item>(m_widgets);
		}

	private:
		Box m_box;
		TupleFromEnum<ControlId, EnumTypeTraits> m_widgets;
	};
}

#endif