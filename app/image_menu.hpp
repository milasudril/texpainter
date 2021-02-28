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
		static constexpr char const* description() { return "Creates a new image"; }
	};

	template<>
	struct ImageActionTraits<ImageAction::Import>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Import"; }
		static constexpr char const* description()
		{
			return "Imports an existing image from a file";
		}
	};

	template<>
	struct ImageActionTraits<ImageAction::Export>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Export"; }
		static constexpr char const* description() { return "Exports an current image to a file"; }
	};

	template<>
	struct ImageActionTraits<ImageAction::Delete>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Delete"; }
		static constexpr char const* description() { return "Deletes current image"; }
	};
}

#endif