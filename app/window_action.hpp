//@	{"targets":[{"name":"window_action.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_APPWINDOWS_HPP
#define TEXPAINTER_APP_APPWINDOWS_HPP

#include "ui/menu_item.hpp"

#include "libenum/empty.hpp"

namespace Texpainter
{
	enum class WindowAction : int
	{
		ImageEditor,
		FilterGraphEditor,
		DocumentPreviewer
	};

	constexpr auto begin(Enum::Empty<WindowAction>) { return WindowAction::ImageEditor; }

	constexpr auto end(Enum::Empty<WindowAction>)
	{
		return static_cast<WindowAction>(static_cast<int>(WindowAction::DocumentPreviewer) + 1);
	}

	template<WindowAction>
	struct WindowActionMenuTraits;

	template<>
	struct WindowActionMenuTraits<WindowAction::ImageEditor>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Image editor"; }
	};

	template<>
	struct WindowActionMenuTraits<WindowAction::FilterGraphEditor>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Compositor"; }
	};

	template<>
	struct WindowActionMenuTraits<WindowAction::DocumentPreviewer>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Document preview"; }
	};
}

#endif