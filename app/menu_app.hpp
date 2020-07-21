//@	{"targets":[{"name":"menu_app.hpp","type":"include"}]}

#ifndef TEXPAINTER_MENUAPP_HPP
	#define TEXPAITNER_MENUAPP_HPP

	#include "ui/menu_item.hpp"
	#include "utils/empty.hpp"

namespace Texpainter
{
	enum class AppAction : int
	{
		Help,
		About,
		Quit
	};

	constexpr auto end(Empty<AppAction>)
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
	};

	template<>
	struct AppActionTraits<AppAction::About>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "About"; }
	};

	template<>
	struct AppActionTraits<AppAction::Quit>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Quit"; }
	};
}

#endif