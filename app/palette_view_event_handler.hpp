//@	{
//@	"targets":[{"name":"palette_view_event_handler.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_PALETTEVIEWEVENTHANDLER_HPP
#define TEXPAINTER_PALETTEVIEWEVENTHANDLER_HPP

#include "./menu_palette.hpp"

#include "utils/polymorphic_rng.hpp"
#include "model/document.hpp"
#include "ui/dialog.hpp"
#include "ui/palette_view.hpp"

#include <random>

namespace Texpainter
{
	template<class DocOwner>
	class PaletteViewEventHandler
	{
		using PaletteCreatorDlg = Ui::Dialog<Ui::LabeledInput<Ui::TextEntry>>;

	public:
		explicit PaletteViewEventHandler(Ui::Container& dialog_owner,
		                                 DocOwner& doc_owner,
		                                 PolymorphicRng rng)
		    : r_dlg_owner{dialog_owner}
		    , r_doc_owner{doc_owner}
		    , m_rng{rng}
		{
		}

		template<auto>
		void onMouseDown(Texpainter::Ui::PaletteView& view, Model::ColorIndex, int)
		{
		}

		template<auto>
		void onMouseUp(Texpainter::Ui::PaletteView& pal_view, Model::ColorIndex index, int button)
		{
			switch(button)
			{
				case 1:
					(void)r_doc_owner.documentModify([this, &pal_view, index](auto& doc) {
						auto current_color = doc.currentColor();
						doc.currentColor(index);
						pal_view.highlightMode(current_color, Ui::PaletteView::HighlightMode::None)
						    .highlightMode(index, Ui::PaletteView::HighlightMode::Read)
						    .update();
						return false;
					});
					break;

				case 3: printf("Right\n"); break;
			}
		}

		template<auto>
		void onMouseMove(Texpainter::Ui::PaletteView&, Model::ColorIndex)
		{
		}

	private:
		Ui::Container& r_dlg_owner;
		DocOwner& r_doc_owner;
		PolymorphicRng m_rng;
	};
}

#endif