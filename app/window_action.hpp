//@	{"targets":[{"name":"window_action.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_WINDOWACTION_HPP
#define TEXPAINTER_APP_WINDOWACTION_HPP

#include "ui/menu_item.hpp"

#include "libenum/empty.hpp"

namespace Texpainter
{
	enum class WindowAction : int
	{
		ImageEditor,
		Compositor,
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
		static constexpr char const* displayName() { return "Show image editor"; }
	};

	template<>
	struct WindowActionMenuTraits<WindowAction::Compositor>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Show compositor"; }
	};

	template<>
	struct WindowActionMenuTraits<WindowAction::DocumentPreviewer>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Show document preview"; }
	};
}

#endif