//@	{"targets":[{"name":"main_menu.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_MAINMENU_HPP
#define TEXPAINTER_APP_MAINMENU_HPP

#include "./app_menu.hpp"
#include "./workspace_action.hpp"
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
		Workspace,
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

		static constexpr char const* description()
		{
			return "This menu contains options not related a document, such as exiting the "
			       "application, or showing the online help.";
		}

		using type = Ui::SubmenuBuilder<AppAction, AppActionTraits>;
	};

	template<>
	struct MainMenuItemTraits<MainMenuItem::Workspace>
	{
		static constexpr char const* displayName() { return "Workspace"; }

		static constexpr char const* description()
		{
			return "This menu contains options related to the workspace, such as showing different "
			       "windows and rearranging them.";
		}

		using type = Ui::SubmenuBuilder<WorkspaceAction, WorkspaceActionMenuTraits>;
	};

	template<>
	struct MainMenuItemTraits<MainMenuItem::Document>
	{
		static constexpr char const* displayName() { return "Document"; }

		static constexpr char const* description()
		{
			return "This menu contains document-related options, such as Open, Save and export.";
		}

		using type = Ui::SubmenuBuilder<DocumentAction, DocumentActionTraits>;
	};

	template<>
	struct MainMenuItemTraits<MainMenuItem::Image>
	{
		static constexpr char const* displayName() { return "Image"; }

		static constexpr char const* description()
		{
			return "This menu contains image-related options, such as creating a new image, or "
			       "deleting an existing one.";
		}

		using type = Ui::SubmenuBuilder<ImageAction, ImageActionTraits>;
	};

	template<>
	struct MainMenuItemTraits<MainMenuItem::Palette>
	{
		static constexpr char const* displayName() { return "Palette"; }

		static constexpr char const* description()
		{
			return "This menu contains palette-related options, such as creating a new image, or "
			       "deleting an existing one.";
		}


		using type = Ui::SubmenuBuilder<PaletteAction, PaletteActionTraits>;
	};

	template<PaletteAction>
	struct PaletteActionTraits;
}

#endif