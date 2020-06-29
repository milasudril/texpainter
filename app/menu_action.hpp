//@	{"targets":[{"name":"menu_action.hpp","type":"include"}]}

#ifndef TEXPAINTER_MENUACTION_HPP
#define TEXPAITNER_MENUACTION_HPP

#include "ui/button.hpp"
#include "ui/separator.hpp"

namespace Texpainter
{
	enum class MenuAction : int
	{
		New,
		Open,
		Save,
		Export,
		GeneratorsSep,
		GenSolidColor,
		SelectBrush,
		GenNoise,
		GenCracks,
		GenVoronoi
	};

	constexpr auto end(Texpainter::Empty<MenuAction>)
	{
		return static_cast<MenuAction>(static_cast<int>(MenuAction::GenVoronoi) + 1);
	}

	template<MenuAction action>
	struct MenuActionTraits;

	template<>
	struct MenuActionTraits<MenuAction::New>
	{
		using type = Texpainter::Ui::Button;
		static constexpr char const* displayName()
		{
			return "New";
		}
	};

	template<>
	struct MenuActionTraits<MenuAction::Open>
	{
		using type = Texpainter::Ui::Button;
		static constexpr char const* displayName()
		{
			return "Open";
		}
	};

	template<>
	struct MenuActionTraits<MenuAction::Save>
	{
		using type = Texpainter::Ui::Button;
		static constexpr char const* displayName()
		{
			return "Save";
		}
	};

	template<>
	struct MenuActionTraits<MenuAction::Export>
	{
		using type = Texpainter::Ui::Button;
		static constexpr char const* displayName()
		{
			return "Export";
		}
	};

	template<>
	struct MenuActionTraits<MenuAction::GeneratorsSep>
	{
		using type = Texpainter::Ui::Separator;
		static constexpr char const* displayName()
		{
			return "Export";
		}
	};

	template<>
	struct MenuActionTraits<MenuAction::GenSolidColor>
	{
		using type = Texpainter::Ui::Button;
		static constexpr char const* displayName()
		{
			return "Gen. solid color";
		}
	};

	template<>
	struct MenuActionTraits<MenuAction::SelectBrush>
	{
		using type = Texpainter::Ui::Button;
		static constexpr char const* displayName()
		{
			return "Select brush";
		}
	};

	template<>
	struct MenuActionTraits<MenuAction::GenNoise>
	{
		using type = Texpainter::Ui::Button;
		static constexpr char const* displayName()
		{
			return "Gen. noise";
		}
	};

	template<>
	struct MenuActionTraits<MenuAction::GenCracks>
	{
		using type = Texpainter::Ui::Button;
		static constexpr char const* displayName()
		{
			return "Gen. cracks";
		}
	};

	template<>
	struct MenuActionTraits<MenuAction::GenVoronoi>
	{
		using type = Texpainter::Ui::Button;
		static constexpr char const* displayName()
		{
			return "Gen. voronoi";
		}
	};
}

#endif