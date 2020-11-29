//@	{"targets":[{"name":"image_selector.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_IMAGESELECTOR_HPP
#define TEXPAINTER_APP_IMAGESELECTOR_HPP

#include "ui/combobox.hpp"

#include <ranges>
#include <algorithm>

namespace Texpainter::App
{
	class ImageSelector
	{
	public:
		explicit ImageSelector(Ui::Container& owner): m_selector{owner} {}

		ImageSelector& clear()
		{
			m_selector.clear();
			return *this;
		}

		template<std::ranges::input_range Source>
		ImageSelector& appendFrom(Source&& src)
		{
			std::ranges::for_each(src, [&sel = m_selector](auto item) { sel.append(item); });
			return *this;
		}

		ImageSelector& selected(int val)
		{
			m_selector.selected(val);
			return *this;
		}

		template<auto id, class EventHandler>
		ImageSelector& eventHandler(EventHandler& eh)
		{
			m_selector.eventHandler<id>(eh);
			return *this;
		}

	private:
		Ui::Combobox m_selector;
	};
}

#endif