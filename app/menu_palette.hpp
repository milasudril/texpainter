//@	{"targets":[{"name":"menu_palette.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_MENUPALETTE_HPP
#define TEXPAINTER_APP_MENUPALETTE_HPP

#include "ui/menu_item.hpp"
#include "ui/menu_builder.hpp"

namespace Texpainter
{
	enum class PaletteActionNew : int
	{
		Empty,
		Generate
	};

	constexpr auto end(Empty<PaletteActionNew>)
	{
		return static_cast<PaletteActionNew>(static_cast<int>(PaletteActionNew::Generate) + 1);
	}

	template<PaletteActionNew>
	struct PaletteActionNewTraits;

	template<>
	struct PaletteActionNewTraits<PaletteActionNew::Empty>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Empty"; }
	};

	template<>
	struct PaletteActionNewTraits<PaletteActionNew::Generate>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Generate"; }
	};

	enum class PaletteAction : int
	{
		New,
		Load,
		Store,
		Rename,
		Remove
	};

	constexpr auto end(Empty<PaletteAction>)
	{
		return static_cast<PaletteAction>(static_cast<int>(PaletteAction::Remove) + 1);
	}

	template<PaletteAction>
	struct PaletteActionTraits;

	template<>
	struct PaletteActionTraits<PaletteAction::New>
	{
		using type = Ui::SubmenuBuilder<PaletteActionNew, PaletteActionNewTraits>;
		static constexpr char const* displayName() { return "New"; }
	};

	template<>
	struct PaletteActionTraits<PaletteAction::Load>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Load"; }
	};

	template<>
	struct PaletteActionTraits<PaletteAction::Store>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Store"; }
	};

	template<>
	struct PaletteActionTraits<PaletteAction::Rename>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Rename"; }
	};

	template<>
	struct PaletteActionTraits<PaletteAction::Remove>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Remove"; }
	};
}

#endif