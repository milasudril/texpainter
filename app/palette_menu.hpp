//@	{"targets":[{"name":"palette_menu.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_PALETTEMENU_HPP
#define TEXPAINTER_APP_PALETTEMENU_HPP

#include "ui/menu_item.hpp"

#include "libenum/empty.hpp"

namespace Texpainter
{
	enum class PaletteAction : int
	{
		New,
		Generate,
		Import,
		Export,
		Delete
	};

	constexpr auto begin(Enum::Empty<PaletteAction>) { return PaletteAction::New; }

	constexpr auto end(Enum::Empty<PaletteAction>)
	{
		return static_cast<PaletteAction>(static_cast<int>(PaletteAction::Delete) + 1);
	}

	template<PaletteAction>
	struct PaletteActionTraits;

	template<>
	struct PaletteActionTraits<PaletteAction::New>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "New"; }
		static constexpr char const* description() { return "Creates a new and empty palette"; }
	};

	template<>
	struct PaletteActionTraits<PaletteAction::Generate>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Generate"; }
		static constexpr char const* description()
		{
			return "Generates a new palette by using the palette creator tool";
		}
	};

	template<>
	struct PaletteActionTraits<PaletteAction::Import>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Import"; }
		static constexpr char const* description() { return "Imports an existing palette file"; }
	};

	template<>
	struct PaletteActionTraits<PaletteAction::Export>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Export"; }
		static constexpr char const* description()
		{
			return "Exports the current palette to a file";
		}
	};

	template<>
	struct PaletteActionTraits<PaletteAction::Delete>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Delete"; }
		static constexpr char const* description() { return "Deletes the current palette"; }
	};
}

#endif