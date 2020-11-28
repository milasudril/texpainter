//@	{"targets":[{"name":"main_menu.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_MAINMENU_HPP
#define TEXPAINTER_APP_MAINMENU_HPP

#include "./app_menu.hpp"
#include "./app_windows.hpp"
#include "./document_menu.hpp"
#include "./image_menu.hpp"
#include "./palette_menu.hpp"

#include "ui/menu_item.hpp"
#include "ui/menu_builder.hpp"

namespace Texpainter
{
	enum class MainMenuItem : int
	{
		Application,
		Windows,
		Document,
		Image,
		Palette
	};

	constexpr auto begin(Enum::Empty<MainMenuItem>) { return MainMenuItem::Application; }

	constexpr auto end(Enum::Empty<MainMenuItem>)
	{
		return static_cast<MainMenuItem>(static_cast<int>(MainMenuItem::Palette) + 1);
	}

	template<MainMenuItem>
	struct MainMenuItemTraits;

	template<>
	struct MainMenuItemTraits<MainMenuItem::Application>
	{
		static constexpr char const* displayName() { return "Application"; }

		using type = Ui::SubmenuBuilder<AppAction, AppActionTraits>;
	};

	template<>
	struct MainMenuItemTraits<MainMenuItem::Windows>
	{
		static constexpr char const* displayName() { return "Windows"; }

		using type = Ui::SubmenuBuilder<AppWindowType, AppWindowTypeMenuTraits>;
	};

	template<>
	struct MainMenuItemTraits<MainMenuItem::Document>
	{
		static constexpr char const* displayName() { return "Document"; }

		using type = Ui::SubmenuBuilder<DocumentAction, DocumentActionTraits>;
	};

	template<>
	struct MainMenuItemTraits<MainMenuItem::Image>
	{
		static constexpr char const* displayName() { return "Image"; }

		using type = Ui::SubmenuBuilder<ImageAction, ImageActionTraits>;
	};

	template<>
	struct MainMenuItemTraits<MainMenuItem::Palette>
	{
		static constexpr char const* displayName() { return "Palette"; }

		using type = Ui::SubmenuBuilder<PaletteAction, PaletteActionTraits>;
	};

	template<PaletteAction>
	struct PaletteActionTraits;
}

#endif