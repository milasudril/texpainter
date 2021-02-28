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
		static constexpr char const* description() { return "Creates a new document"; }
	};

	template<>
	struct DocumentActionTraits<DocumentAction::Open>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Open"; }
		static constexpr char const* description() { return "Opens an existing document"; }
	};

	template<>
	struct DocumentActionTraits<DocumentAction::Save>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Save"; }
		static constexpr char const* description() { return "Saves current document"; }
	};


	template<>
	struct DocumentActionTraits<DocumentAction::SaveAs>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Save As"; }
		static constexpr char const* description()
		{
			return "Saves current document under a new name";
		}
	};

	template<>
	struct DocumentActionTraits<DocumentAction::SaveCopy>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Save copy"; }
		static constexpr char const* description()
		{
			return "Saves current document under a new name, but keeps the original \"open\"";
		}
	};

	template<>
	struct DocumentActionTraits<DocumentAction::Export>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Export"; }
		static constexpr char const* description()
		{
			return "Renders current document and saves the result to an image file";
		}
	};

	template<>
	struct DocumentActionTraits<DocumentAction::SetCanvasSize>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Set canvas size"; }
		static constexpr char const* description() { return "Sets a new canvas size"; }
	};

	template<>
	struct DocumentActionTraits<DocumentAction::RenderToImage>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Render to image"; }
		static constexpr char const* description()
		{
			return "Renders current document to an image";
		}
	};
}

#endif