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
#include "ui/separator.hpp"
#include "ui/slider.hpp"
#include "ui/keyboard_state.hpp"

#include <numbers>

namespace Texpainter
{
	class AppWindow
	{
	public:
		enum class ControlId : int
		{
			LayerSelector,
			BrushSelector,
			BrushSize,
			PaletteSelector,
			Canvas
		};

		explicit AppWindow(Ui::Container& container, PolymorphicRng rng);

		AppWindow& document(Model::Document&& doc)
		{
			m_current_document = std::make_unique<Model::Document>(std::move(doc));
			m_pal_view.eventHandler<0>(m_pal_view_eh);
			m_layer_selector.inputField().eventHandler<ControlId::LayerSelector>(*this);
			m_brush_selector.inputField().eventHandler<ControlId::BrushSelector>(*this);
			m_brush_size.eventHandler<ControlId::BrushSize>(*this);
			m_palette_selector.inputField().eventHandler<ControlId::PaletteSelector>(*this);
			m_img_view.eventHandler<ControlId::Canvas>(*this);
			update();
			m_rows.killFocus();
			return *this;
		}

		template<Mutator<Model::Document> Func>
		bool documentModify(Func&& f)
		{
			auto res = f(*m_current_document);
			if(res) { update(); }
			m_rows.killFocus();
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

		template<LayerAction action>
		void onActivated(Ui::MenuItem& item)
		{
			if(hasDocument()) { m_layer_menu_handler.onActivated<action>(item); }
		}

		template<LayerActionNew action>
		void onActivated(Ui::MenuItem& item)
		{
			if(hasDocument()) { m_layer_menu_handler.onActivated<action>(item); }
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
		void onChanged(Ui::Slider& src);

		template<ControlId>
		void onMouseUp(Ui::ImageView& view, vec2_t pos_window, vec2_t pos_screen, int button);

		template<ControlId>
		void onMouseDown(Ui::ImageView& view, vec2_t pos_window, vec2_t pos_screen, int button);

		template<ControlId>
		void onMouseMove(Ui::ImageView& view, vec2_t pos_window, vec2_t pos_screen);

		template<ControlId>
		void onKeyDown(Ui::ImageView& view, int scancode)
		{
		}

		template<ControlId>
		void onKeyUp(Ui::ImageView& view, int scancode)
		{
		}

		void onKeyDown(Ui::Scancode key)
		{
			if(key == GrabKey || key == RotateKey || key == ScaleKey) { m_key_state.press(key); }
		}

		void onKeyUp(Ui::Scancode key)
		{
			m_key_state.release(key);  // No operation if key not in set
			printf("Release %u\n", key.value());
		}

	private:
		std::unique_ptr<Model::Document> m_current_document;

		PolymorphicRng m_rng;
		uint32_t m_mouse_state;
		Ui::KeyboardState m_key_state;
		static constexpr auto MouseButtonLeft  = 0x1;
		static constexpr auto MouseButtonRight = 0x4;
		static constexpr auto GrabKey          = Ui::Scancode{34};
		static constexpr auto RotateKey        = Ui::Scancode{16};
		static constexpr auto ScaleKey         = Ui::Scancode{31};

		DocMenuHandler<AppWindow> m_doc_menu_handler;
		LayerMenuHandler<AppWindow> m_layer_menu_handler;
		PaletteMenuHandler<AppWindow> m_palette_menu_handler;
		PaletteViewEventHandler<AppWindow> m_pal_view_eh;

		Ui::Box m_rows;
		Ui::MenuBuilder<MainMenuItem, MainMenuItemTraits> m_menu;
		Ui::Box m_selectors;
		Ui::LabeledInput<Ui::Combobox> m_layer_selector;
		Ui::Separator m_sep0;
		Ui::LabeledInput<Ui::Combobox> m_brush_selector;
		Ui::Slider m_brush_size;
		Ui::Separator m_sep1;
		Ui::LabeledInput<Ui::Combobox> m_palette_selector;
		Ui::PaletteView m_pal_view;
		Ui::Label m_layer_info;
		Ui::ImageView m_img_view;


		void updateLayerSelector();
		void updateBrushSelector();
		void updatePaletteSelector();
		void updateLayerInfo();
		void update();
		void doRender();
		void paint(vec2_t pos);
		void grab(vec2_t pos);
	};

	template<>
	inline void AppWindow::onChanged<AppWindow::ControlId::LayerSelector>(Ui::Combobox& src)
	{
		auto const index   = Model::LayerIndex{static_cast<uint32_t>(src.selected())};
		auto const& layers = m_current_document->layers();
		if(auto layer_name = layers.key(index); layer_name != nullptr)
		{
			m_current_document->currentLayer(std::string{*layer_name});
			updateLayerInfo();
			doRender();
		}
		m_rows.killFocus();
	}


	template<>
	inline void AppWindow::onChanged<AppWindow::ControlId::BrushSelector>(Ui::Combobox& src)
	{
		auto brush      = m_current_document->currentBrush();
		auto brush_type = static_cast<Model::BrushType>(
		    static_cast<int>(end(Empty<Model::BrushType>{})) - 1 - src.selected());
		brush.type(brush_type);
		m_current_document->currentBrush(brush);
		m_rows.killFocus();
	}

	template<>
	inline void AppWindow::onChanged<AppWindow::ControlId::BrushSize>(Ui::Slider& src)
	{
		auto brush = m_current_document->currentBrush();
		brush.radius(static_cast<float>(logValue(src.value())));
		m_current_document->currentBrush(brush);
		m_rows.killFocus();
	}


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
		m_rows.killFocus();
	}
}

#endif