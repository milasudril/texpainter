//@	{"targets":[{"name":"menu_file.hpp","type":"include"}]}

#ifndef TEXPAINTER_MENUFILE_HPP
#define TEXPAITNER_MENUFILE_HPP

#include "ui/menu_item.hpp"

namespace Texpainter
{
	enum class FileAction : int
	{
		New,
		Open,
		Save,
		SaveAs,
		SaveCopy,
		Export,
		SetCanvasSize
	};

	constexpr auto end(Empty<FileAction>)
	{
		return static_cast<FileAction>(static_cast<int>(FileAction::SetCanvasSize) + 1);
	}

	template<FileAction>
	struct FileActionTraits;

	template<>
	struct FileActionTraits<FileAction::New>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName()
		{
			return "New";
		}
	};

	template<>
	struct FileActionTraits<FileAction::Open>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName()
		{
			return "Open";
		}
	};

	template<>
	struct FileActionTraits<FileAction::Save>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName()
		{
			return "Save";
		}
	};


	template<>
	struct FileActionTraits<FileAction::SaveAs>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName()
		{
			return "Save As";
		}
	};

	template<>
	struct FileActionTraits<FileAction::SaveCopy>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName()
		{
			return "Save copy";
		}
	};

	template<>
	struct FileActionTraits<FileAction::Export>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName()
		{
			return "Export";
		}
	};

	template<>
	struct FileActionTraits<FileAction::SetCanvasSize>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName()
		{
			return "Set canvas size";
		}
	};
}

#endif