//@	{"targets":[{"name":"workspace_action.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_WORKSPACEACTION_HPP
#define TEXPAINTER_APP_WORKSPACEACTION_HPP

#include "ui/menu_item.hpp"

#include "libenum/empty.hpp"

namespace Texpainter
{
	enum class WorkspaceAction : int
	{
		ShowImageEditor,
		ShowCompositor,
		ShowDocumentPreviewer,
		ResetLayout
	};

	constexpr auto begin(Enum::Empty<WorkspaceAction>) { return WorkspaceAction::ShowImageEditor; }

	constexpr auto end(Enum::Empty<WorkspaceAction>)
	{
		return static_cast<WorkspaceAction>(static_cast<int>(WorkspaceAction::ResetLayout) + 1);
	}

	template<WorkspaceAction>
	struct WorkspaceActionMenuTraits;

	template<>
	struct WorkspaceActionMenuTraits<WorkspaceAction::ShowImageEditor>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Show image editor"; }
	};

	template<>
	struct WorkspaceActionMenuTraits<WorkspaceAction::ShowCompositor>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Show compositor"; }
	};

	template<>
	struct WorkspaceActionMenuTraits<WorkspaceAction::ShowDocumentPreviewer>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Show document preview"; }
	};

	template<>
	struct WorkspaceActionMenuTraits<WorkspaceAction::ResetLayout>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Reset layout"; }
	};
}

#endif