//@	{"targets":[{"name":"palette_selector.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_PALETTESELECTOR_HPP
#define TEXPAINTER_APP_PALETTESELECTOR_HPP

#include "model/palette.hpp"
#include "model/item_name.hpp"

#include "ui/combobox.hpp"
#include "ui/palette_view.hpp"

namespace Texpainter::App
{
	class PaletteSelector
	{
	public:
		explicit PaletteSelector(Ui::Container& owner)
		    : m_root{owner, Ui::Box::Orientation::Horizontal}
		    , m_name{m_root}
		    , m_palette{m_root.insertMode(Ui::Box::InsertMode{0, Ui::Box::Fill | Ui::Box::Expand})}
		{
		}

		template<auto id, class EventHandler>
		PaletteSelector& eventHandler(EventHandler& eh)
		{
			m_name.eventHandler<id>(eh);
			m_palette.eventHandler<id>(eh);
			return *this;
		}

		PaletteSelector& clear()
		{
			m_name.clear();
			m_palette.palette(std::span<PixelStore::Pixel>{});
			return *this;
		}

		template<std::ranges::input_range Source>
		PaletteSelector& appendFrom(Source&& src)
		{
			std::ranges::for_each(src, [&sel = m_name](auto item) { sel.append(item); });
			return *this;
		}

		template<class PaletteMap>
		requires requires(PaletteMap a)
		{
			a.find(std::declval<Model::ItemName>());
		}
		PaletteSelector& selected(Model::ItemName const& name, PaletteMap const& pals)
		{
			m_name.selected(name.c_str());
			if(auto i = pals.find(name); i != std::end(pals)) [[likely]]
				{
					m_palette.palette(i->second.source.get());
				}
			return *this;
		}

		PaletteSelector& highlightMode(PixelStore::ColorIndex index,
		                               Ui::PaletteView::HighlightMode mode)
		{
			m_palette.highlightMode(index, mode).update();
			return *this;
		}

	private:
		Ui::Box m_root;
		Ui::Combobox m_name;
		Ui::PaletteView m_palette;
	};
}

#endif