//@	{
//@  "targets":[{"name":"menu_builder.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UI_MENUBUILDER_HPP
#define TEXPAINTER_UI_MENUBUILDER_HPP

#include "./menu_bar.hpp"
#include "./menu_item.hpp"
#include "./submenu.hpp"
#include "./checkable_menu_item.hpp"

#include "libenum/enum.hpp"
#include "libenum/tuple.hpp"

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
			Enum::forEachEnumItem<EnumType>([this](auto tag) {
				m_items.template get<tag.value>().label(EnumTypeTraits<tag.value>::displayName());
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
			Enum::forEachEnumItem<EnumType>([this, &eh](auto tag) {
				if constexpr(
				    std::is_same_v<
				        Ui::MenuItem,
				        typename EnumTypeTraits<tag.value>::
				            type> || std::is_same_v<Ui::CheckableMenuItem, typename EnumTypeTraits<tag.value>::type>)
				{ m_items.template get<tag.value>().template eventHandler<tag.value>(eh); }
				else
				{
					m_items.template get<tag.value>().eventHandler(eh);
				}
			});
			return *this;
		}

		auto& items() { return m_items; }

	private:
		Submenu m_root;
		Enum::Tuple<EnumType, EnumTypeTraits> m_items;
	};

	template<class EnumType, template<EnumType> class EnumTypeTraits>
	class MenuBuilder
	{
	public:
		using ControlId = EnumType;

		explicit MenuBuilder(Container& owner): m_root{owner}, m_items{m_root}
		{
			Enum::forEachEnumItem<EnumType>([this](auto tag) {
				m_items.template get<tag.value>().label(EnumTypeTraits<tag.value>::displayName());
			});
		}

		template<class EventHandler>
		MenuBuilder& eventHandler(EventHandler& eh)
		{
			Enum::forEachEnumItem<EnumType>([this, &eh](auto tag) {
				if constexpr(
				    std::is_same_v<
				        Ui::MenuItem,
				        typename EnumTypeTraits<tag.value>::
				            type> || std::is_same_v<Ui::CheckableMenuItem, typename EnumTypeTraits<tag.value>::type>)
				{ m_items.template get<tag.value>().template eventHandler<tag.value>(eh); }
				else
				{
					m_items.template get<tag.value>().eventHandler(eh);
				}
			});
			return *this;
		}

		auto& items() { return m_items; }

	private:
		MenuBar m_root;
		Enum::Tuple<EnumType, EnumTypeTraits> m_items;
	};
}

#endif