//@	{
//@	"targets":[{"name":"palette_view_event_handler.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_APP_PALETTEVIEWEVENTHANDLER_HPP
#define TEXPAINTER_APP_PALETTEVIEWEVENTHANDLER_HPP

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
		class ColorPickerData
		{
		public:
			explicit ColorPickerData(Ui::PaletteView& pal_view,
			                         PixelStore::ColorIndex current_index,
			                         Model::Document& doc)
			    : r_palette_view{pal_view}
			    , m_current_index{current_index}
			    , r_document{doc}
			{
			}

			Model::Document& document() { return r_document.get(); }

			PixelStore::ColorIndex currentIndex() const { return m_current_index; }

			Ui::PaletteView& paletteView() { return r_palette_view.get(); }

		private:
			std::reference_wrapper<Ui::PaletteView> r_palette_view;
			PixelStore::ColorIndex m_current_index;
			std::reference_wrapper<Model::Document> r_document;
		};

		using ColorPicker = Texpainter::Ui::Dialog<InheritFrom<ColorPickerData, Ui::ColorPicker>>;

	public:
		explicit PaletteViewEventHandler(Ui::Container& dialog_owner, PolymorphicRng rng)
		    : m_rng{rng}
		    , r_dlg_owner{dialog_owner}
		{
		}

		template<auto>
		void onMouseUp(Texpainter::Ui::PaletteView& pal_view,
		               PixelStore::ColorIndex index,
		               int button,
		               Model::Document& doc)
		{
			switch(button)
			{
				case 1:
					doc.layersModify([&view = pal_view, index, &doc](auto& layers) {
						if(auto current_layer = layers[doc.currentLayer()];
						   current_layer != nullptr)
						{
							auto current_color = current_layer->currentColor();
							current_layer->currentColor(index);
							view.highlightMode(current_color, Ui::PaletteView::HighlightMode::None)
							    .highlightMode(index, Ui::PaletteView::HighlightMode::Read)
							    .update();
							return true;
						}
						return false;
					});
					break;

				case 3:
					pal_view.highlightMode(index, Texpainter::Ui::PaletteView::HighlightMode::Write)
					    .update();
					m_color_picker = std::make_unique<ColorPicker>(
					    ColorPickerData{pal_view, index, doc},
					    r_dlg_owner,
					    (std::string{"Select color number "} + std::to_string(index.value() + 1))
					        .c_str(),
					    m_rng,
					    "Recently used: ",
					    m_color_history);
					m_color_picker->template eventHandler<0>(*this).widget().value(
					    pal_view.color(index));
					break;
			}
		}

		template<auto>
		void confirmPositive(ColorPicker& src)
		{
			auto const color_new = src.widget().value();
			src.widget().document().layersModify([&doc      = src.widget().document(),
			                                      sel_index = src.widget().currentIndex(),
			                                      color_new = color_new](auto& layers) {
				if(auto current_layer = layers[doc.currentLayer()]; current_layer != nullptr)
				{
					current_layer->colorModify(sel_index, color_new);
					return true;
				}
				return false;
			});

			src.widget()
			    .paletteView()
			    .color(src.widget().currentIndex(), color_new)
			    .highlightMode(src.widget().currentIndex(),
			                   Texpainter::Ui::PaletteView::HighlightMode::None)
			    .highlightMode(currentLayer(src.widget().document())->currentColor(),
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
			    .paletteView()
			    .highlightMode(src.widget().currentIndex(),
			                   Texpainter::Ui::PaletteView::HighlightMode::None)
			    .highlightMode(currentLayer(src.widget().document())->currentColor(),
			                   Texpainter::Ui::PaletteView::HighlightMode::Read)
			    .update();

			m_color_picker.reset();
		}

		template<auto id>
		void handleException(char const* msg, ColorPicker&)
		{
			fprintf(stderr, "Error occured in ColorPicker: %s\n", msg);
			std::terminate();
		}

	private:
		PolymorphicRng m_rng;
		std::array<PixelStore::Pixel, 8> m_color_history;

		Ui::Container& r_dlg_owner;
		std::unique_ptr<ColorPicker> m_color_picker;
	};
}

#endif