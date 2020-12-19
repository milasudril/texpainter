//@	{"targets":[{"name":"image_menu.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_IMAGEMENU_HPP
#define TEXPAINTER_APP_IMAGEMENU_HPP

#include "ui/menu_item.hpp"

#include "libenum/empty.hpp"

namespace Texpainter
{
	enum class ImageAction : int
	{
		New,
		Import,
		Export,
		Delete
	};

	constexpr auto begin(Enum::Empty<ImageAction>) { return ImageAction::New; }

	constexpr auto end(Enum::Empty<ImageAction>)
	{
		return static_cast<ImageAction>(static_cast<int>(ImageAction::Delete) + 1);
	}

	template<ImageAction>
	struct ImageActionTraits;

	template<>
	struct ImageActionTraits<ImageAction::New>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "New"; }
	};

	template<>
	struct ImageActionTraits<ImageAction::Import>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Import"; }
	};

	template<>
	struct ImageActionTraits<ImageAction::Export>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Export"; }
	};

	template<>
	struct ImageActionTraits<ImageAction::Delete>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Delete"; }
	};
}

#endif