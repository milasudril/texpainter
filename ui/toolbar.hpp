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
	template<class EnumType, template<EnumType> class EnumTypeTraits, class EventHandler>
	class Toolbar
	{
	public:
		using ControlId = EnumType;

		explicit Toolbar(Container& owner, Box::Orientation orientation, EventHandler& eh): m_box{owner, orientation},
		m_eh{eh},
		m_widgets{*this}
		{
		}

		template<ControlId id>
		decltype(auto) create()
		{
			auto ret = typename EnumTypeTraits<id>::type{m_box, EnumTypeTraits<id>::name};
			ret.template eventHandler<id>(m_eh);
			return ret;
		}

	private:
		Box m_box;
		EventHandler& m_eh;
		TupleFromEnum<ControlId, EnumTypeTraits> m_widgets;
	};
}

#endif