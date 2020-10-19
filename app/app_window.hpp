//@	{
//@	"targets":[{"name":"app_window.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"app_window.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_APP_APPWINDOW_HPP
#define TEXPAINTER_APP_APPWINDOW_HPP

#include "./menu_action.hpp"
#include "./doc_menu_handler.hpp"
#include "./layer_menu_handler.hpp"
#include "./palette_view_event_handler.hpp"
#include "./filter_graph_editor.hpp"
#include "./document_manager.hpp"

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
#include "ui/error_message_dialog.hpp"

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
			PaletteView,
			Canvas
		};

		explicit AppWindow(Ui::Container& container, PolymorphicRng rng);

		bool hasDocument() const { return m_documents.currentDocument() != nullptr; }


		template<AppAction>
		void onActivated(Ui::MenuItem&)
		{
		}

		template<FileAction action>
		void onActivated(Ui::MenuItem& item)
		{
			m_doc_menu_handler.onActivated(item, m_documents, MenuActionCallback<action>{*this});
		}

		template<LayerAction action>
		void onActivated(Ui::MenuItem& item)
		{
			if(hasDocument())
			{
				m_layer_menu_handler.onActivated(
				    item, *m_documents.currentDocument(), MenuActionCallback<action>{*this});
			}
		}

		template<LayerAction action>
		void onActivated(Ui::CheckableMenuItem& item)
		{
			if(hasDocument())
			{
				m_layer_menu_handler.onActivated(
				    item, *m_documents.currentDocument(), MenuActionCallback<action>{*this});
			}
			else
			{
				item.toggle();
			}
		}

		template<LayerActionNew action>
		void onActivated(Ui::MenuItem& item)
		{
			if(hasDocument())
			{
				m_layer_menu_handler.onActivated(
				    item, *m_documents.currentDocument(), MenuActionCallback<action>{*this});
			}
		}

		template<LayerActionClearTransformation action>
		void onActivated(Ui::MenuItem& item)
		{
			if(hasDocument())
			{
				m_layer_menu_handler.onActivated(
				    item, *m_documents.currentDocument(), MenuActionCallback<action>{*this});
			}
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
		void onMouseDown(Ui::PaletteView&, PixelStore::ColorIndex, int)
		{
		}

		template<ControlId>
		void onMouseUp(Ui::PaletteView&, PixelStore::ColorIndex, int button);

		template<ControlId>
		void onMouseMove(Ui::PaletteView&, PixelStore::ColorIndex)
		{
		}

		template<LayerAction>
		void onUpdated(CompositingOptionsEditor const&);

		void onKeyDown(Ui::Scancode key);

		void onKeyUp(Ui::Scancode key);

		template<class T>
		void operator()(T)
		{
			update();
		}

		void operator()(Tag<LayerActionNew::Empty>)
		{
			auto& current_layer = *currentLayer(*m_documents.currentDocument());
			m_pal_view.inputField()
			    .palette(current_layer.palette())
			    .highlightMode(current_layer.currentColor(),
			                   Texpainter::Ui::PaletteView::HighlightMode::Read);

			update();
		}

		void operator()(Tag<FileAction::New>)
		{
			m_pal_view.inputField().eventHandler<ControlId::PaletteView>(*this);
			m_layer_selector.inputField().eventHandler<ControlId::LayerSelector>(*this);
			m_brush_selector.inputField().eventHandler<ControlId::BrushSelector>(*this);
			m_brush_size.eventHandler<ControlId::BrushSize>(*this);
			m_img_view.eventHandler<ControlId::Canvas>(*this);
			m_paint_info.content("");
			update();
			m_rows.killFocus();
		}

		template<auto, class... T>
		void handleException(char const* msg, T&...)
		{
			m_err_disp.show(m_rows, "Texpainter", msg);
		}

	private:
		DocumentManager m_documents;

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

		DocMenuHandler m_doc_menu_handler;
		LayerMenuHandler m_layer_menu_handler;
		PaletteViewEventHandler m_pal_view_eh;

		Ui::Box m_rows;
		Ui::MenuBuilder<MainMenuItem, MainMenuItemTraits> m_menu;
		Ui::Box m_selectors;
		Ui::LabeledInput<Ui::Combobox> m_layer_selector;
		Ui::Separator m_sep0;
		Ui::LabeledInput<Ui::Combobox> m_brush_selector;
		Ui::Slider m_brush_size;
		Ui::Separator m_sep1;
		Ui::LabeledInput<Ui::PaletteView> m_pal_view;
		Ui::Box m_info;
		Ui::Label m_layer_info;
		Ui::Label m_paint_info;
		Ui::ImageView m_img_view;
		Ui::ErrorMessageDialog m_err_disp;

		void updateLayerSelector();
		void updateBrushSelector();
		void updateLayerInfo();
		void update();
		void doRender(Model::CompositingOptions const& compose_opts);
		void doRender()
		{
			if(auto current_layer = currentLayer(*m_documents.currentDocument());
			   current_layer != nullptr)
			{ doRender(current_layer->compositingOptions()); }
			else
			{
				m_img_view.clear();
			}
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
		auto const index       = Model::LayerIndex{static_cast<uint32_t>(src.selected())};
		auto& current_document = *m_documents.currentDocument();
		auto const& layers     = current_document.layers();
		if(auto layer_name = layers.key(index); layer_name != nullptr)
		{
			current_document.currentLayer(Model::ItemName{*layer_name});
			updateLayerInfo();
			m_pal_view.inputField().palette(currentLayer(current_document)->palette());
			doRender();
		}
		m_rows.killFocus();
	}


	template<>
	inline void AppWindow::onChanged<AppWindow::ControlId::BrushSelector>(Ui::Combobox& src)
	{
		auto& current_document = *m_documents.currentDocument();
		auto brush             = current_document.currentBrush();
		auto brush_type        = static_cast<Model::BrushType>(
            static_cast<int>(end(Empty<Model::BrushType>{})) - 1 - src.selected());
		brush.type(brush_type);
		current_document.currentBrush(brush);
		m_rows.killFocus();
	}

	template<>
	inline void AppWindow::onChanged<AppWindow::ControlId::BrushSize>(Ui::Slider& src)
	{
		auto& current_document = *m_documents.currentDocument();
		auto brush             = current_document.currentBrush();
		brush.radius(static_cast<float>(logValue(src.value())));
		current_document.currentBrush(brush);
		m_rows.killFocus();
	}

	template<>
	inline void AppWindow::onUpdated<LayerAction::CompositingOptions>(
	    CompositingOptionsEditor const& src)
	{
		doRender(src.compositingOptions());
	}


	template<>
	inline void AppWindow::onMouseUp<AppWindow::ControlId::PaletteView>(
	    Ui::PaletteView& src, PixelStore::ColorIndex index, int button)
	{
		auto& current_document = *m_documents.currentDocument();
		m_pal_view_eh.template onMouseUp<AppWindow::ControlId::PaletteView>(
		    src, index, button, current_document);
	}
}

#endif