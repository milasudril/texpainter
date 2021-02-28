//@	{"targets":[{"name":"app_menu.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_APPMENU_HPP
#define TEXPAINTER_APP_APPMENU_HPP

#include "ui/menu_item.hpp"

#include "libenum/empty.hpp"

namespace Texpainter
{
	enum class AppAction : int
	{
		Help,
		About,
		Quit
	};

	constexpr auto begin(Enum::Empty<AppAction>) { return AppAction::Help; }

	constexpr auto end(Enum::Empty<AppAction>)
	{
		return static_cast<AppAction>(static_cast<int>(AppAction::Quit) + 1);
	}

	template<AppAction>
	struct AppActionTraits;

	template<>
	struct AppActionTraits<AppAction::Help>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Help"; }
		static constexpr char const* description()
		{
			return "Opens the online help in the default web browser";
		}
	};

	template<>
	struct AppActionTraits<AppAction::About>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "About"; }
		static constexpr char const* description()
		{
			return "Opens the about page in the default web browser";
		}
	};

	template<>
	struct AppActionTraits<AppAction::Quit>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Quit"; }
		static constexpr char const* description() { return "Exits the application"; }
	};
}

#endif