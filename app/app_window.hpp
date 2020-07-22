//@	{
//@	"targets":[{"name":"app_window.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"app_window.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_APPWINDOW_HPP
#define TEXPAINTER_APPWINDOW_HPP

#include "./menu_action.hpp"
#include "./doc_menu_handler.hpp"
#include "./layer_menu_handler.hpp"
#include "./palette_menu_handler.hpp"
#include "./palette_view_event_handler.hpp"

#include "model/document.hpp"
#include "utils/snap.hpp"
#include "ui/box.hpp"
#include "ui/image_view.hpp"
#include "ui/labeled_input.hpp"
#include "ui/dialog.hpp"
#include "ui/menu_builder.hpp"
#include "ui/palette_view.hpp"
#include "ui/combobox.hpp"

#include <numbers>

namespace Texpainter
{
	class AppWindow
	{
	public:
		enum class ControlId : int
		{
			LayerSelector,
			PaletteSelector
		};

		explicit AppWindow(Ui::Container& container, PolymorphicRng rng)
		    :  // State
		    m_rng{rng}

		    // Event handlers
		    , m_doc_menu_handler{container, *this}
		    , m_layer_menu_handler{container, *this, m_rng}
		    , m_palette_menu_handler{container, *this, m_rng}
		    , m_pal_view_eh{container, *this, m_rng}

		    // Widgets
		    , m_rows{container, Ui::Box::Orientation::Vertical}
		    , m_menu{m_rows}
		    , m_selectors{m_rows, Ui::Box::Orientation::Horizontal}
		    , m_layer_selector{m_selectors, Ui::Box::Orientation::Horizontal, "Layer: "}
		    , m_palette_selector{m_selectors, Ui::Box::Orientation::Horizontal, "Palettes: "}
		    , m_pal_view{m_selectors.insertMode(
		          Ui::Box::InsertMode{4, Ui::Box::Fill | Ui::Box::Expand})}
		    , m_img_view{m_rows.insertMode(Ui::Box::InsertMode{0, Ui::Box::Fill | Ui::Box::Expand})}
		{
			//	m_img_view.eventHandler<ControlId::Canvas>(*this);
			m_menu.eventHandler(*this);
		}

		AppWindow& document(Model::Document&& doc)
		{
			m_current_document = std::make_unique<Model::Document>(std::move(doc));
			m_pal_view.eventHandler<0>(m_pal_view_eh);
			m_layer_selector.inputField().eventHandler<ControlId::LayerSelector>(*this);
			m_palette_selector.inputField().eventHandler<ControlId::PaletteSelector>(*this);
			update();
			return *this;
		}

		template<Mutator<Model::Document> Func>
		bool documentModify(Func&& f)
		{
			auto res = f(*m_current_document);
			if(res) { update(); }
			return res;
		}

		Model::Document const& document() const { return *m_current_document; }

		bool hasDocument() const { return m_current_document != nullptr; }


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
		void onActivated(Ui::MenuItem& item)
		{
			m_palette_menu_handler.onActivated<action>(item);
		}

		template<ControlId>
		void onChanged(Ui::Combobox& src);

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

		PolymorphicRng m_rng;
		DocMenuHandler<AppWindow> m_doc_menu_handler;
		LayerMenuHandler<AppWindow> m_layer_menu_handler;
		PaletteMenuHandler<AppWindow> m_palette_menu_handler;
		PaletteViewEventHandler<AppWindow> m_pal_view_eh;

		Ui::Box m_rows;
		Ui::MenuBuilder<MainMenuItem, MainMenuItemTraits> m_menu;
		Ui::Box m_selectors;
		Ui::LabeledInput<Ui::Combobox> m_layer_selector;
		Ui::LabeledInput<Ui::Combobox> m_palette_selector;
		Ui::PaletteView m_pal_view;
		Ui::ImageView m_img_view;

		void updateLayerSelector()
		{
			auto& layer_selector = m_layer_selector.inputField();

			layer_selector.clear();
			std::ranges::for_each(
			    m_current_document->layers().keysByIndex(),
			    [&layer_selector](auto const& name) { layer_selector.append(name.c_str()); });

			auto const& current_layer    = m_current_document->currentLayer();
			auto const current_layer_idx = m_current_document->layers().index(current_layer);
			layer_selector.selected(static_cast<int>(current_layer_idx));
		}

		void updatePaletteSelector()
		{
			auto& pal_selector = m_palette_selector.inputField();

			pal_selector.clear();
			std::ranges::for_each(
			    m_current_document->palettes().keys(),
			    [&pal_selector](auto const& name) { pal_selector.append(name.c_str()); });

			auto const& current_pal_name = m_current_document->currentPalette();
			auto const current_layer_idx =
			    m_current_document->palettes().position(current_pal_name);
			pal_selector.selected(static_cast<int>(current_layer_idx));

			if(auto pal = currentPalette(*m_current_document); pal != nullptr)
			{
				m_pal_view.palette(*pal)
				    .highlightMode(m_current_document->currentColor(),
				                   Ui::PaletteView::HighlightMode::Read)
				    .update();
			}
			else
			{
				Model::Pixel color{0.0f, 0.0f, 0.0f, 0.0f};
				m_pal_view.palette(std::span{&color, 1}).update();
			}
		}


		void update()
		{
			updateLayerSelector();
			updatePaletteSelector();
			doRender();
		}

		void doRender()
		{
			Model::Image canvas{m_current_document->canvasSize()};
			std::ranges::fill(canvas.pixels(), Model::Pixel{0.0f, 0.0f, 0.0f, 0.0f});
			std::ranges::for_each(m_current_document->layersByIndex(),
			                      [&canvas](auto const& layer) {
				                      if(layer.visible()) { render(layer, canvas); }
			                      });

			if(auto current_layer = currentLayer(*m_current_document); current_layer != nullptr)
				[[likely]] { outline(*current_layer, canvas); }
			m_img_view.image(canvas);
		}
	};

	template<>
	inline void AppWindow::onChanged<AppWindow::ControlId::PaletteSelector>(Ui::Combobox& src)
	{
		auto const index     = Model::PaletteIndex{static_cast<uint32_t>(src.selected())};
		auto const& palettes = m_current_document->palettes();
		if(auto pal_name = palettes.key(index); pal_name != nullptr)
		{
			m_current_document->currentPalette(std::string{*pal_name});
			m_pal_view.palette(*palettes[index])
			    .highlightMode(m_current_document->currentColor(),
			                   Ui::PaletteView::HighlightMode::Read);
		}
	}

	template<>
	inline void AppWindow::onChanged<AppWindow::ControlId::LayerSelector>(Ui::Combobox& src)
	{
		auto const index   = Model::LayerIndex{static_cast<uint32_t>(src.selected())};
		auto const& layers = m_current_document->layers();
		if(auto layer_name = layers.key(index); layer_name != nullptr)
		{
			m_current_document->currentLayer(std::string{*layer_name});
			doRender();
		}
	}
}

#endif