//@	{
//@	"targets":[{"name":"palette_view_event_handler.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_APP_PALETTEVIEWEVENTHANDLER_HPP
#define TEXPAINTER_APP_PALETTEVIEWEVENTHANDLER_HPP

#include "utils/polymorphic_rng.hpp"
#include "utils/default_rng.hpp"
#include "model/document.hpp"
#include "ui/dialog.hpp"
#include "ui/palette_view.hpp"
#include "ui/color_picker.hpp"
#include "utils/function_ref.hpp"
#include "utils/empty.hpp"

#include <random>

namespace Texpainter
{
	class PaletteViewEventHandler
	{
	public:
		template<auto ControlId>
		class Callback: public FunctionRef<void(Ui::PaletteView&)>
		{
		public:
			template<class T>
			explicit Callback(T& obj) requires(!std::same_as<std::decay_t<T>, Callback>)
			    : FunctionRef<void(Ui::PaletteView&)>(obj, Tag<ControlId>{})
			{
			}
		};

	private:
		class ColorPickerData
		{
		public:
			explicit ColorPickerData(Ui::PaletteView& pal_view,
			                         PixelStore::ColorIndex current_index,
			                         Model::Document& doc,
			                         FunctionRef<void(Ui::PaletteView&)> cb)
			    : r_palette_view{pal_view}
			    , m_current_index{current_index}
			    , r_document{doc}
			    , m_cb{cb}
			{
			}

			Model::Document& document() { return r_document.get(); }

			PixelStore::ColorIndex currentIndex() const { return m_current_index; }

			Ui::PaletteView& paletteView() { return r_palette_view.get(); }

			void onCompleted() const { m_cb(r_palette_view.get()); }

		private:
			std::reference_wrapper<Ui::PaletteView> r_palette_view;
			PixelStore::ColorIndex m_current_index;
			std::reference_wrapper<Model::Document> r_document;
			FunctionRef<void(Ui::PaletteView&)> m_cb;
		};

		using ColorPicker = Texpainter::Ui::Dialog<InheritFrom<ColorPickerData, Ui::ColorPicker>>;

	public:
		explicit PaletteViewEventHandler(Ui::Container& dialog_owner): r_dlg_owner{dialog_owner} {}

		template<auto id>
		void onMouseUp(Texpainter::Ui::PaletteView& pal_view,
		               PixelStore::ColorIndex index,
		               int button,
		               Model::Document& doc,
		               Callback<id> cb)
		{
			switch(button)
			{
				case 1:
					modifyCurrentLayer(doc, [&view = pal_view, index](auto& layer) {
						auto const current_color = layer.currentColor();
						layer.currentColor(index);
						(void)view
						    .highlightMode(current_color, Ui::PaletteView::HighlightMode::None)
						    .highlightMode(index, Ui::PaletteView::HighlightMode::Read)
						    .update();
						return true;
					});
					break;

				case 3:
					pal_view.highlightMode(index, Texpainter::Ui::PaletteView::HighlightMode::Write)
					    .update();
					m_color_picker = std::make_unique<ColorPicker>(
					    ColorPickerData{pal_view, index, doc, cb},
					    r_dlg_owner,
					    (std::string{"Select color number "} + std::to_string(index.value() + 1))
					        .c_str(),
					    PolymorphicRng{DefaultRng::engine()},
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
			src.widget().paletteView().highlightMode(
			    currentLayer(src.widget().document())->currentColor(),
			    Texpainter::Ui::PaletteView::HighlightMode::None);
			auto const color_new = src.widget().value();
			(void)modifyCurrentLayer(
			    src.widget().document(),
			    [sel_index = src.widget().currentIndex(), color_new = color_new](auto& layer) {
				    layer.colorModify(sel_index, color_new).currentColor(sel_index);
				    return true;
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
			src.widget().onCompleted();
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
		std::array<PixelStore::Pixel, 8> m_color_history;

		Ui::Container& r_dlg_owner;
		std::unique_ptr<ColorPicker> m_color_picker;
	};
}

#endif