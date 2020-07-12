//@	{"targets":[{"name":"menu_action.hpp","type":"include"}]}

#ifndef TEXPAINTER_MENUACTION_HPP
#define TEXPAITNER_MENUACTION_HPP

#include "./menu_app.hpp"
#include "./menu_file.hpp"
#include "./menu_layer.hpp"
#include "./menu_palette.hpp"

#include "ui/menu_item.hpp"
#include "ui/menu_builder.hpp"

namespace Texpainter
{
	enum class MainMenuItem : int
	{
		Application,
		File,
		Layer,
		Palette
	};

	constexpr auto end(Empty<MainMenuItem>)
	{
		return static_cast<MainMenuItem>(static_cast<int>(MainMenuItem::Palette) + 1);
	}

	template<MainMenuItem>
	struct MainMenuItemTraits;

	template<>
	struct MainMenuItemTraits<MainMenuItem::Application>
	{
		static constexpr char const* displayName()
		{
			return "Application";
		}

		using type = Ui::SubmenuBuilder<AppAction, AppActionTraits>;
	};

	template<>
	struct MainMenuItemTraits<MainMenuItem::File>
	{
		static constexpr char const* displayName()
		{
			return "File";
		}

		using type = Ui::SubmenuBuilder<FileAction, FileActionTraits>;
	};

	template<>
	struct MainMenuItemTraits<MainMenuItem::Layer>
	{
		static constexpr char const* displayName()
		{
			return "Layer";
		}

		using type = Ui::SubmenuBuilder<LayerAction, LayerActionTraits>;
	};

	template<LayerAction>
	struct LayerActionTraits;

	template<>
	struct MainMenuItemTraits<MainMenuItem::Palette>
	{
		static constexpr char const* displayName()
		{
			return "Palette";
		}

		using type = Ui::SubmenuBuilder<PaletteAction, PaletteActionTraits>;
	};

	template<PaletteAction>
	struct PaletteActionTraits;


	enum class MenuAction : int
	{
		New,
		Open,
		Save,
		Export,
		GeneratorsSep,
		GenSolidColor,
		SelectBrush,
		GenNoise,
		GenCracks,
		GenVoronoi
	};

	constexpr auto end(Texpainter::Empty<MenuAction>)
	{
		return static_cast<MenuAction>(static_cast<int>(MenuAction::GenVoronoi) + 1);
	}

	template<MenuAction action>
	struct MenuActionTraits;

	template<>
	struct MenuActionTraits<MenuAction::New>
	{
		using type = Texpainter::Ui::Button;
		static constexpr char const* displayName()
		{
			return "New";
		}
	};

	template<>
	struct MenuActionTraits<MenuAction::Open>
	{
		using type = Texpainter::Ui::Button;
		static constexpr char const* displayName()
		{
			return "Open";
		}
	};

	template<>
	struct MenuActionTraits<MenuAction::Save>
	{
		using type = Texpainter::Ui::Button;
		static constexpr char const* displayName()
		{
			return "Save";
		}
	};

	template<>
	struct MenuActionTraits<MenuAction::Export>
	{
		using type = Texpainter::Ui::Button;
		static constexpr char const* displayName()
		{
			return "Export";
		}
	};

	template<>
	struct MenuActionTraits<MenuAction::GeneratorsSep>
	{
		using type = Texpainter::Ui::Separator;
		static constexpr char const* displayName()
		{
			return "Export";
		}
	};

	template<>
	struct MenuActionTraits<MenuAction::GenSolidColor>
	{
		using type = Texpainter::Ui::Button;
		static constexpr char const* displayName()
		{
			return "Gen. solid color";
		}
	};

	template<>
	struct MenuActionTraits<MenuAction::SelectBrush>
	{
		using type = Texpainter::Ui::Button;
		static constexpr char const* displayName()
		{
			return "Select brush";
		}
	};

	template<>
	struct MenuActionTraits<MenuAction::GenNoise>
	{
		using type = Texpainter::Ui::Button;
		static constexpr char const* displayName()
		{
			return "Gen. noise";
		}
	};

	template<>
	struct MenuActionTraits<MenuAction::GenCracks>
	{
		using type = Texpainter::Ui::Button;
		static constexpr char const* displayName()
		{
			return "Gen. cracks";
		}
	};

	template<>
	struct MenuActionTraits<MenuAction::GenVoronoi>
	{
		using type = Texpainter::Ui::Button;
		static constexpr char const* displayName()
		{
			return "Gen. voronoi";
		}
	};
}

#endif