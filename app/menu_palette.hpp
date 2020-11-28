//@	{"targets":[{"name":"menu_palette.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_MENUPALETTE_HPP
#define TEXPAINTER_APP_MENUPALETTE_HPP

#include "ui/menu_item.hpp"
#include "ui/menu_builder.hpp"

namespace Texpainter
{
	enum class PaletteAction : int
	{
		Clear,
		Generate,
		Import,
		Export
	};

	constexpr auto begin(Enum::Empty<PaletteAction>) { return PaletteAction::Clear; }

	constexpr auto end(Enum::Empty<PaletteAction>)
	{
		return static_cast<PaletteAction>(static_cast<int>(PaletteAction::Export) + 1);
	}

	template<PaletteAction>
	struct PaletteActionTraits;

	template<>
	struct PaletteActionTraits<PaletteAction::Clear>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Clear"; }
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
}

#endif