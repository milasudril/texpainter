//@	{
//@	"targets":[{"name":"app_window.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"app_window.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_APPWINDOW_HPP
#define TEXPAINTER_APPWINDOW_HPP

#include "./menu_action.hpp"
#include "./doc_menu_handler.hpp"
#include "./layer_menu_handler.hpp"
#include "./palette_editor.hpp"

#include "model/document.hpp"
#include "utils/snap.hpp"
#include "ui/box.hpp"
#include "ui/image_view.hpp"
#include "ui/labeled_input.hpp"
#include "ui/dialog.hpp"
#include "ui/menu_builder.hpp"

#include <numbers>

namespace Texpainter
{
	class AppWindow
	{
	public:
		enum class ControlId : int
		{
			PaletteEd,
			LayerStackCtrl,
			Canvas
		};

		explicit AppWindow(Ui::Container& container):
		   m_doc_menu_handler{container, *this},
		   m_layer_menu_handler{container, *this},
		   m_rows{container, Ui::Box::Orientation::Vertical},
		   m_menu{m_rows},
		   m_selectors{m_rows, Ui::Box::Orientation::Horizontal},
		   m_layer_selector{m_selectors, Ui::Box::Orientation::Horizontal, "Layer: "},
		   m_pal_editor{m_selectors.insertMode(Ui::Box::InsertMode{4, Ui::Box::Fill | Ui::Box::Expand}),
		                Ui::Box::Orientation::Horizontal,
		                "Palette: "},
		   m_img_view{m_rows.insertMode(Ui::Box::InsertMode{0, Ui::Box::Fill | Ui::Box::Expand})}
		{
			m_pal_editor.inputField().eventHandler<ControlId::PaletteEd>(*this);
			//	m_img_view.eventHandler<ControlId::Canvas>(*this);
			m_menu.eventHandler(*this);
		}

		AppWindow& document(Model::Document&& doc)
		{
			m_current_document = std::make_unique<Model::Document>(std::move(doc));
			doRender();
			return *this;
		}

		template<Mutator<Model::Document> Func>
		bool documentModify(Func&& f)
		{
			auto res = f(*m_current_document);
			if(res) { doRender(); }
			return res;
		}

		Model::Document const& document() const
		{
			return *m_current_document;
		}

		bool hasDocument() const
		{
			return m_current_document != nullptr;
		}


		template<AppAction>
		void onActivated(Ui::MenuItem&)
		{
		}

		template<FileAction action>
		void onActivated(Ui::MenuItem& item)
		{
			m_doc_menu_handler.onActivated<action>(item);
		}

		template<LayerAction>
		void onActivated(Ui::MenuItem&)
		{
		}

		template<LayerActionNew action>
		void onActivated(Ui::MenuItem& item)
		{
			m_layer_menu_handler.onActivated<action>(item);
		}

		template<LayerActionClearTransformation>
		void onActivated(Ui::MenuItem&)
		{
		}

		template<PaletteAction>
		void onActivated(Ui::MenuItem&)
		{
		}

		template<PaletteActionNew action>
		void onActivated(Ui::MenuItem&)
		{
			if constexpr(action == PaletteActionNew::Empty)
			{ m_pal_editor.inputField().createEmptyPalette(); }
			/*	else
			 { m_pal_editor.generatePalette(); }*/
		}

		template<ControlId>
		void onChanged(PaletteEditor& pal)
		{
			//			m_current_color = pal.selectedPalette()[pal.selectedColorIndex()];
			m_img_view.focus();
		}

		template<ControlId>
		void onMouseUp(Ui::ImageView& view, vec2_t pos_window, vec2_t pos_screen, int button);

		template<ControlId>
		void onMouseDown(Ui::ImageView& view, vec2_t pos_window, vec2_t pos_screen, int button);

		template<ControlId>
		void onMouseMove(Ui::ImageView& view, vec2_t pos_window, vec2_t pos_screen);

		template<ControlId>
		void onKeyDown(Ui::ImageView& view, int scancode);

		template<ControlId>
		void onKeyUp(Ui::ImageView& view, int scancode);

	private:
		std::unique_ptr<Model::Document> m_current_document;
		DocMenuHandler<AppWindow> m_doc_menu_handler;
		LayerMenuHandler<AppWindow> m_layer_menu_handler;

		Ui::Box m_rows;
		Ui::MenuBuilder<MainMenuItem, MainMenuItemTraits> m_menu;
		Ui::Box m_selectors;
		Ui::LabeledInput<Ui::Combobox> m_layer_selector;
		Ui::LabeledInput<PaletteEditor> m_pal_editor;
		Ui::ImageView m_img_view;


		void doRender()
		{
			Model::Image canvas{m_current_document->canvasSize()};
			std::ranges::fill(canvas.pixels(), Model::Pixel{0.0f, 0.0f, 0.0f, 0.0f});
			std::ranges::for_each(m_current_document->layersByIndex(), [&canvas](auto const& layer) {
				if(layer.visible()) { render(layer, canvas); }
			});

			// TODO:	m_layerstack_ctrl.inputField().outlineCurrentLayer(canvas.pixels());
			m_img_view.image(canvas);
		}
	};
}

#endif