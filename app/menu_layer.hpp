//@	{"targets":[{"name":"menu_layer.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_MENULAYER_HPP
#define TEXPAINTER_APP_MENULAYER_HPP

#include "ui/menu_item.hpp"
#include "ui/checkable_menu_item.hpp"
#include "ui/menu_builder.hpp"

namespace Texpainter
{
	enum class LayerActionNew : int
	{
		Empty,
		FromFile
	};

	constexpr auto end(Empty<LayerActionNew>)
	{
		return static_cast<LayerActionNew>(static_cast<int>(LayerActionNew::FromFile) + 1);
	}

	template<LayerActionNew>
	struct LayerActionNewTraits;

	template<>
	struct LayerActionNewTraits<LayerActionNew::Empty>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Empty"; }
	};

	template<>
	struct LayerActionNewTraits<LayerActionNew::FromFile>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "From file"; }
	};

	enum class LayerAction : int
	{
		New,
		Copy,
		Link,
		LinkToCopy,
		Rename,
		Delete,
		ClearTransformation,
		MoveToTop,
		MoveUp,
		MoveDown,
		MoveToBottom,
		CompositingOptions,
		Isolate,
		Hide
	};

	constexpr auto end(Empty<LayerAction>)
	{
		return static_cast<LayerAction>(static_cast<int>(LayerAction::Hide) + 1);
	}

	template<LayerAction>
	struct LayerActionTraits;

	template<>
	struct LayerActionTraits<LayerAction::New>
	{
		using type = Ui::SubmenuBuilder<LayerActionNew, LayerActionNewTraits>;
		static constexpr char const* displayName() { return "New"; }
	};

	template<>
	struct LayerActionTraits<LayerAction::Copy>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Create copy"; }
	};

	template<>
	struct LayerActionTraits<LayerAction::Link>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Create link"; }
	};


	template<>
	struct LayerActionTraits<LayerAction::LinkToCopy>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Convert link to copy"; }
	};

	template<>
	struct LayerActionTraits<LayerAction::Rename>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Rename"; }
	};

	template<>
	struct LayerActionTraits<LayerAction::Delete>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Delete"; }
	};

	enum class LayerActionClearTransformation : int
	{
		Rotation,
		Location,
		Scale,
		All
	};

	constexpr auto end(Empty<LayerActionClearTransformation>)
	{
		return static_cast<LayerActionClearTransformation>(
		    static_cast<int>(LayerActionClearTransformation::All) + 1);
	}

	template<LayerActionClearTransformation>
	struct LayerActionClearTransformationTraits;

	template<>
	struct LayerActionClearTransformationTraits<LayerActionClearTransformation::Rotation>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Rotation"; }
	};

	template<>
	struct LayerActionClearTransformationTraits<LayerActionClearTransformation::Location>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Location"; }
	};

	template<>
	struct LayerActionClearTransformationTraits<LayerActionClearTransformation::Scale>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Scale"; }
	};

	template<>
	struct LayerActionClearTransformationTraits<LayerActionClearTransformation::All>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "All"; }
	};

	template<>
	struct LayerActionTraits<LayerAction::ClearTransformation>
	{
		using type = Ui::SubmenuBuilder<LayerActionClearTransformation,
		                                LayerActionClearTransformationTraits>;
		static constexpr char const* displayName() { return "Clear transformation"; }
	};


	template<>
	struct LayerActionTraits<LayerAction::MoveToTop>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Move to top"; }
	};

	template<>
	struct LayerActionTraits<LayerAction::MoveUp>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Move up"; }
	};

	template<>
	struct LayerActionTraits<LayerAction::MoveDown>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Move down"; }
	};

	template<>
	struct LayerActionTraits<LayerAction::MoveToBottom>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Move to bottom"; }
	};


	template<>
	struct LayerActionTraits<LayerAction::CompositingOptions>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Compositing options"; }
	};

	template<>
	struct LayerActionTraits<LayerAction::Isolate>
	{
		using type = Ui::CheckableMenuItem;
		static constexpr char const* displayName() { return "Isolate"; }
	};

	template<>
	struct LayerActionTraits<LayerAction::Hide>
	{
		using type = Ui::CheckableMenuItem;
		static constexpr char const* displayName() { return "Hide"; }
	};
}

#endif