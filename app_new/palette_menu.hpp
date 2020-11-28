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
	};

	template<>
	struct PaletteActionTraits<PaletteAction::Generate>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Generate"; }
	};

	template<>
	struct PaletteActionTraits<PaletteAction::Import>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Import"; }
	};

	template<>
	struct PaletteActionTraits<PaletteAction::Export>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Export"; }
	};

	template<>
	struct PaletteActionTraits<PaletteAction::Delete>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Delete"; }
	};
}

#endif