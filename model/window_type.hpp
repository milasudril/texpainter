#ifndef TEXPAINTER_MODEL_WINDOWTYPE_HPP
#define TEXPAINTER_MODEL_WINDOWTYPE_HPP

#include "libenum/empty.hpp"

namespace Texpainter::Model
{
	enum class WindowType : int
	{
		ImageEditor,
		Compositor,
		DocumentPreviewer
	};

	constexpr auto begin(Enum::Empty<WindowType>) { return WindowType::ImageEditor; }

	constexpr auto end(Enum::Empty<WindowType>)
	{
		return WindowType{static_cast<int>(WindowType::DocumentPreviewer) + 1};
	}
}

#endif