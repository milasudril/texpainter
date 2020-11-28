//@	{"targets":[{"name":"menu_action.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_MENUACTION_HPP
#define TEXPAINTER_APP_MENUACTION_HPP

#include "./menu_app.hpp"
#include "./menu_file.hpp"
#include "./menu_layer.hpp"
#include "./menu_palette.hpp"

#include "ui/menu_item.hpp"
#include "ui/menu_builder.hpp"

#include "libenum/enum.hpp"

namespace Texpainter
{
	enum class MainMenuItem : int
	{
		Application,
		File,
		Layer,
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
	struct MainMenuItemTraits<MainMenuItem::File>
	{
		static constexpr char const* displayName() { return "Document"; }

		using type = Ui::SubmenuBuilder<FileAction, FileActionTraits>;
	};

	template<>
	struct MainMenuItemTraits<MainMenuItem::Layer>
	{
		static constexpr char const* displayName() { return "Layer"; }

		using type = Ui::SubmenuBuilder<LayerAction, LayerActionTraits>;
	};

	template<LayerAction>
	struct LayerActionTraits;

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