//@	{
//@  "targets":[{"name":"menu_builder.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UI_MENUBUILDER_HPP
#define TEXPAINTER_UI_MENUBUILDER_HPP

#include "./menu_bar.hpp"
#include "./menu_item.hpp"
#include "./submenu.hpp"

#include "utils/empty.hpp"

#include <type_traits>
#include <memory>

namespace Texpainter::Ui
{
	template<class EnumType, template<EnumType> class EnumTypeTraits>
	class SubmenuBuilder
	{
	public:
		using ControlId = EnumType;

		explicit SubmenuBuilder(Container& owner): m_root{owner}, m_items{m_root.menu()}
		{
			forEachEnumItem<EnumType>([this](auto tag) {
				get<tag.value>(m_items).label(EnumTypeTraits<tag.value>::displayName());
			});
		}

		SubmenuBuilder& label(char const* lab)
		{
			m_root.label(lab);
			return *this;
		}

		template<class EventHandler>
		SubmenuBuilder& eventHandler(EventHandler& eh)
		{
			forEachEnumItem<EnumType>([this, &eh](auto tag) {
				if constexpr(std::is_same_v<Ui::MenuItem, typename EnumTypeTraits<tag.value>::type>)
				{ get<tag.value>(m_items).template eventHandler<tag.value>(eh); }
				else
				{
					get<tag.value>(m_items).eventHandler(eh);
				}
			});
			return *this;
		}

	private:
		Submenu m_root;
		TupleFromEnum<EnumType, EnumTypeTraits> m_items;
	};

	template<class EnumType, template<EnumType> class EnumTypeTraits>
	class MenuBuilder
	{
	public:
		using ControlId = EnumType;

		explicit MenuBuilder(Container& owner): m_root{owner}, m_items{m_root}
		{
			forEachEnumItem<EnumType>([this](auto tag) {
				get<tag.value>(m_items).label(EnumTypeTraits<tag.value>::displayName());
			});
		}

		template<class EventHandler>
		MenuBuilder& eventHandler(EventHandler& eh)
		{
			forEachEnumItem<EnumType>([this, &eh](auto tag) {
				if constexpr(std::is_same_v<Ui::MenuItem, typename EnumTypeTraits<tag.value>::type>)
				{ get<tag.value>(m_items).template eventHandler<tag.value>(eh); }
				else
				{
					get<tag.value>(m_items).eventHandler(eh);
				}
			});
			return *this;
		}

	private:
		MenuBar m_root;
		TupleFromEnum<EnumType, EnumTypeTraits> m_items;
	};
}

#endif