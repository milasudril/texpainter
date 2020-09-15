//@	{
//@	"targets":[{"name":"palette_view_event_handler.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_APP_PALETTEVIEWEVENTHANDLER_HPP
#define TEXPAINTER_APP_PALETTEVIEWEVENTHANDLER_HPP

#include "./menu_palette.hpp"

#include "utils/polymorphic_rng.hpp"
#include "model/document.hpp"
#include "ui/dialog.hpp"
#include "ui/palette_view.hpp"
#include "ui/color_picker.hpp"

#include <random>

namespace Texpainter
{
	class PaletteViewEventHandler
	{
		struct WrappedColorPicker
		{
			template<class... Args>
			WrappedColorPicker(Args&&... args): first{std::forward<Args>(args)...}
			                                  , second{nullptr}
			{
			}
			Texpainter::Ui::ColorPicker first;
			Ui::PaletteView* second;
		};

		using ColorPicker = Texpainter::Ui::Dialog<WrappedColorPicker>;

	public:
		explicit PaletteViewEventHandler(Ui::Container& dialog_owner, PolymorphicRng rng)
		    : r_document{nullptr}
		    , m_rng{rng}
		    , r_dlg_owner{dialog_owner}
		{
		}

		template<auto>
		void onMouseDown(Texpainter::Ui::PaletteView&, PixelStore::ColorIndex, int)
		{
		}

		template<auto>
		void onMouseUp(Texpainter::Ui::PaletteView& pal_view,
		               PixelStore::ColorIndex index,
		               int button,
		               Model::Document& doc)
		{
			if(auto const current_color = doc.currentColor();
			   current_color.valid() && colorIndexValid(doc, index))
			{
				switch(button)
				{
					case 1:
						doc.currentColor(index);
						pal_view.highlightMode(current_color, Ui::PaletteView::HighlightMode::None)
						    .highlightMode(index, Ui::PaletteView::HighlightMode::Read)
						    .update();
						break;

					case 3:
						r_document       = &doc;
						m_modified_index = index;
						pal_view
						    .highlightMode(index, Texpainter::Ui::PaletteView::HighlightMode::Write)
						    .update();
						m_color_picker =
						    std::make_unique<ColorPicker>(r_dlg_owner,
						                                  (std::string{"Select color number "}
						                                   + std::to_string(index.value() + 1))
						                                      .c_str(),
						                                  m_rng,
						                                  "Recently used: ",
						                                  m_color_history);
						m_color_picker->widget().second = &pal_view;
						m_color_picker->template eventHandler<0>(*this).widget().first.value(
						    pal_view.color(index));
						break;
				}
			}
		}

		template<auto>
		void onMouseMove(Texpainter::Ui::PaletteView&, PixelStore::ColorIndex)
		{
		}

		template<auto>
		void confirmPositive(ColorPicker& src)
		{
			auto const color_new = src.widget().first.value();
			r_document->palettesModify(
			    [color_new    = color_new,
			     sel_index    = m_modified_index,
			     &current_pal = r_document->currentPalette()](auto& palettes) {
				    (*palettes[current_pal])[sel_index] = color_new;
				    return true;
			    });

			src.widget()
			    .second->color(m_modified_index, color_new)
			    .highlightMode(m_modified_index, Texpainter::Ui::PaletteView::HighlightMode::None)
			    .highlightMode(r_document->currentColor(),
			                   Texpainter::Ui::PaletteView::HighlightMode::Read)
			    .update();

			std::rotate(std::rbegin(m_color_history),
			            std::rbegin(m_color_history) + 1,
			            std::rend(m_color_history));
			m_color_history[0] = color_new;
			m_color_picker.reset();
		}

		template<auto>
		void dismiss(ColorPicker& src)
		{
			src.widget()
			    .second
			    ->highlightMode(m_modified_index, Texpainter::Ui::PaletteView::HighlightMode::None)
			    .highlightMode(r_document->currentColor(),
			                   Texpainter::Ui::PaletteView::HighlightMode::Read)
			    .update();

			m_color_picker.reset();
		}

	private:
		Model::Document* r_document;
		PolymorphicRng m_rng;
		PixelStore::ColorIndex m_modified_index;
		std::array<PixelStore::Pixel, 8> m_color_history;

		Ui::Container& r_dlg_owner;
		std::unique_ptr<ColorPicker> m_color_picker;
	};
}

#endif