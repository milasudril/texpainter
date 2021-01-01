//@	{"targets":[{"name":"workspace_action.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_WORKSPACEACTION_HPP
#define TEXPAINTER_APP_WORKSPACEACTION_HPP

#include "ui/menu_item.hpp"

#include "libenum/empty.hpp"

namespace Texpainter
{
	enum class WorkspaceAction : int
	{
		ImageEditor,
		Compositor,
		DocumentPreviewer
	};

	constexpr auto begin(Enum::Empty<WorkspaceAction>) { return WorkspaceAction::ImageEditor; }

	constexpr auto end(Enum::Empty<WorkspaceAction>)
	{
		return static_cast<WorkspaceAction>(static_cast<int>(WorkspaceAction::DocumentPreviewer)
		                                    + 1);
	}

	template<WorkspaceAction>
	struct WorkspaceActionMenuTraits;

	template<>
	struct WorkspaceActionMenuTraits<WorkspaceAction::ImageEditor>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Show image editor"; }
	};

	template<>
	struct WorkspaceActionMenuTraits<WorkspaceAction::Compositor>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Show compositor"; }
	};

	template<>
	struct WorkspaceActionMenuTraits<WorkspaceAction::DocumentPreviewer>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Show document preview"; }
	};
}

#endif