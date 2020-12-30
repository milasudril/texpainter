//@	{"targets":[{"name":"document_menu.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_DOCUMENTMENU_HPP
#define TEXPAINTER_APP_DOCUMENTMENU_HPP

#include "ui/menu_item.hpp"

#include "libenum/empty.hpp"

namespace Texpainter
{
	enum class DocumentAction : int
	{
		New,
		SetCanvasSize,
		Open,
		Save,
		SaveAs,
		SaveCopy,
		RenderToImage,
		Export
	};

	constexpr auto begin(Enum::Empty<DocumentAction>) { return DocumentAction::New; }

	constexpr auto end(Enum::Empty<DocumentAction>)
	{
		return static_cast<DocumentAction>(static_cast<int>(DocumentAction::Export) + 1);
	}

	template<DocumentAction>
	struct DocumentActionTraits;

	template<>
	struct DocumentActionTraits<DocumentAction::New>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "New"; }
	};

	template<>
	struct DocumentActionTraits<DocumentAction::Open>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Open"; }
	};

	template<>
	struct DocumentActionTraits<DocumentAction::Save>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Save"; }
	};


	template<>
	struct DocumentActionTraits<DocumentAction::SaveAs>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Save As"; }
	};

	template<>
	struct DocumentActionTraits<DocumentAction::SaveCopy>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Save copy"; }
	};

	template<>
	struct DocumentActionTraits<DocumentAction::Export>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Export"; }
	};

	template<>
	struct DocumentActionTraits<DocumentAction::SetCanvasSize>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Set canvas size"; }
	};

	template<>
	struct DocumentActionTraits<DocumentAction::RenderToImage>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Render to image"; }
	};
}

#endif