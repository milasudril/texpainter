//@	{"targets":[{"name":"app_windows.hpp","type":"include"}]}

#ifndef TEXPAINTER_APPNEW_APPWINDOWS_HPP
#define TEXPAINTER_APPNEW_APPWINDOWS_HPP

#include "ui/menu_item.hpp"

#include "libenum/empty.hpp"

namespace Texpainter
{
	enum class AppWindowType : int
	{
		ImageEditor,
		FilterGraphEditor,
		Output
	};

	constexpr auto begin(Enum::Empty<AppWindowType>) { return AppWindowType::ImageEditor; }

	constexpr auto end(Enum::Empty<AppWindowType>)
	{
		return static_cast<AppWindowType>(static_cast<int>(AppWindowType::Output) + 1);
	}

	template<AppWindowType>
	struct AppWindowTypeMenuTraits;

	template<>
	struct AppWindowTypeMenuTraits<AppWindowType::ImageEditor>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Image editor"; }
	};

	template<>
	struct AppWindowTypeMenuTraits<AppWindowType::FilterGraphEditor>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Filter graph"; }
	};

	template<>
	struct AppWindowTypeMenuTraits<AppWindowType::Output>
	{
		using type = Ui::MenuItem;
		static constexpr char const* displayName() { return "Output"; }
	};
}

#endif