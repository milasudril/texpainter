//@	{
//@	"targets":[{"name":"app_window.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"app_window.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_APP_APPWINDOW_HPP
#define TEXPAINTER_APP_APPWINDOW_HPP

#include "./menu_action.hpp"
#include "./doc_menu_handler.hpp"
#include "./layer_menu_handler.hpp"
#include "./palette_menu_handler.hpp"
#include "./palette_view_event_handler.hpp"
#include "./filter_graph_editor.hpp"
#include "./compositing_options_editor.hpp"

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
		using CompositingOptionsDlg = Ui::Dialog<CompositingOptionsEditor>;

	public:
		enum class ControlId : int
		{
			LayerSelector,
			BrushSelector,
			BrushSize,
			PaletteSelector,
			Canvas,
			CompositingOptions
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

		template<LayerAction action>
		void onActivated(Ui::CheckableMenuItem& item)
		{
			if(hasDocument()) { m_layer_menu_handler.onActivated<action>(item); }
			else
			{
				item.toggle();
			}
		}

		template<LayerActionNew action>
		void onActivated(Ui::MenuItem& item)
		{
			if(hasDocument()) { m_layer_menu_handler.onActivated<action>(item); }
		}

		template<LayerActionClearTransformation action>
		void onActivated(Ui::MenuItem& item)
		{
			if(hasDocument()) { m_layer_menu_handler.onActivated<action>(item); }
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
		void onMouseUp(Ui::ImageView& view, vec2_t loc_window, vec2_t loc_screen, int button);

		template<ControlId>
		void onMouseDown(Ui::ImageView& view, vec2_t loc_window, vec2_t loc_screen, int button);

		template<ControlId>
		void onMouseMove(Ui::ImageView& view, vec2_t loc_window, vec2_t loc_screen);

		template<ControlId>
		void onKeyDown(Ui::ImageView&, int)
		{
		}

		template<ControlId>
		void onKeyUp(Ui::ImageView&, int)
		{
		}

		template<ControlId>
		void dismiss(CompositingOptionsDlg&);

		template<ControlId>
		void confirmPositive(CompositingOptionsDlg&);

		template<ControlId>
		void onChanged(CompositingOptionsEditor&);

		void onKeyDown(Ui::Scancode key);

		void onKeyUp(Ui::Scancode key);

		void showFxBlendEditor()
		{
			if(auto layer = currentLayer(*m_current_document); layer != nullptr)
			{
				m_fx_blend_editor_dlg =
				    std::make_unique<CompositingOptionsDlg>(m_rows,
				                                            "Compositing options",
				                                            layer->compositingOptions(),
				                                            layer->nodeLocations());
				m_fx_blend_editor_dlg->eventHandler<ControlId::CompositingOptions>(*this);
				m_fx_blend_editor_dlg->widget().eventHandler<ControlId::CompositingOptions>(*this);
			}
		}


	private:
		std::unique_ptr<Model::Document> m_current_document;

		PolymorphicRng m_rng;
		uint32_t m_mouse_state;
		Ui::KeyboardState m_key_state;
		uint32_t m_mod_state;
		Ui::Scancode m_key_prev;

		enum class TransformationMode : int
		{
			Relative,
			Absolute
		};
		TransformationMode m_trans_mode;

		class GrabState
		{
		public:
			GrabState() = default;

			explicit GrabState(vec2_t layer_loc, vec2_t mouse_loc)
			    : m_layer_loc{layer_loc}
			    , m_mouse_loc{mouse_loc}
			{
			}

			vec2_t location(vec2_t loc_current) const
			{
				return m_layer_loc + (loc_current - m_mouse_loc);
			}

			vec2_t initLocation() const { return m_layer_loc; }

		private:
			vec2_t m_layer_loc;
			vec2_t m_mouse_loc;
		} m_grab_state;

		class ScaleState
		{
		public:
			ScaleState() = default;

			explicit ScaleState(vec2_t layer_scale, vec2_t mouse_loc)
			    : m_layer_scale{layer_scale}
			    , m_mouse_loc{mouse_loc}
			{
			}

			template<class SnapFunction>
			vec2_t scaleFactor(vec2_t loc_current, SnapFunction&& f) const
			{
				return m_layer_scale * f(loc_current / m_mouse_loc);
			}

			vec2_t initScale() const { return m_layer_scale; }

		private:
			vec2_t m_layer_scale;
			vec2_t m_mouse_loc;
		} m_scale_state;

		class RotateState
		{
		public:
			RotateState() = default;

			explicit RotateState(Angle layer_rot, Angle mouse_rot)
			    : m_layer_rot{layer_rot}
			    , m_mouse_rot{mouse_rot}
			{
			}

			template<class SnapFunction>
			Angle rotation(Angle ϴ, SnapFunction&& f) const
			{
				return m_layer_rot + f(ϴ - m_mouse_rot);
			}

			Angle initRotation() const { return m_layer_rot; }

		private:
			Angle m_layer_rot;
			Angle m_mouse_rot;
		} m_rot_state;

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
		Ui::Box m_info;
		Ui::Label m_layer_info;
		Ui::Label m_paint_info;
		Ui::ImageView m_img_view;

		std::unique_ptr<CompositingOptionsDlg> m_fx_blend_editor_dlg;


		void updateLayerSelector();
		void updateBrushSelector();
		void updatePaletteSelector();
		void updateLayerInfo();
		void update();
		void doRender(Model::CompositingOptions const& compose_opts);
		void doRender()
		{
			if(auto current_layer = currentLayer(*m_current_document); current_layer != nullptr)
			{ doRender(current_layer->compositingOptions()); }
		}

		void paint(vec2_t loc);

		void grabInit(vec2_t loc, Model::Layer const& current_layer);
		void grab(vec2_t loc);

		void scaleInit(vec2_t loc, Model::Layer const& current_layer);
		void scale(vec2_t loc);

		void rotateInit(vec2_t loc, Model::Layer const& current_layer);
		void rotate(vec2_t loc);

		void printIdleInfo();
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

	template<>
	inline void AppWindow::dismiss<AppWindow::ControlId::CompositingOptions>(CompositingOptionsDlg&)
	{
		doRender();
		m_fx_blend_editor_dlg.reset();
	}

	template<>
	inline void AppWindow::confirmPositive<AppWindow::ControlId::CompositingOptions>(
	    CompositingOptionsDlg& dlg)
	{
		m_current_document->layersModify([&current_layer = m_current_document->currentLayer(),
		                                  &editor        = dlg.widget()](auto& layers) {
			layers[current_layer]->compositingOptions(editor.compositingOptions());
			layers[current_layer]->nodeLocations(editor.nodeLocations());
			return true;
		});
		m_fx_blend_editor_dlg.reset();
	}

	template<>
	inline void AppWindow::onChanged<AppWindow::ControlId::CompositingOptions>(
	    CompositingOptionsEditor& src)
	{
		doRender(src.compositingOptions());
	}
}

#endif