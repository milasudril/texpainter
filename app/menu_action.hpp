//@	{"targets":[{"name":"menu_action.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_MENUACTION_HPP
#define TEXPAINTER_APP_MENUACTION_HPP

#include "./menu_app.hpp"
#include "./menu_file.hpp"
#include "./menu_layer.hpp"

#include "ui/menu_item.hpp"
#include "ui/menu_builder.hpp"

namespace Texpainter
{
	enum class MainMenuItem : int
	{
		Application,
		File,
		Layer
	};

	constexpr auto end(Empty<MainMenuItem>)
	{
		return static_cast<MainMenuItem>(static_cast<int>(MainMenuItem::Layer) + 1);
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
}

#endif