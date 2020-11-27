//@	{"targets":[{"name":"image_selector.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_IMAGESELECTOR_HPP
#define TEXPAINTER_APP_IMAGESELECTOR_HPP

#include "ui/combobox.hpp"

namespace Texpainter::App
{
	class ImageSelector
	{
	public:
		explicit ImageSelector(Ui::Container& owner): m_selector{owner} {}

	private:
		Ui::Combobox m_selector;
	};
}

#endif