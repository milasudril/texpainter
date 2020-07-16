//@	{
//@	"targets":[{"name":"app_window.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"app_window.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_APPWINDOW_HPP
#define TEXPAINTER_APPWINDOW_HPP

#include "./palette_editor.hpp"
#include "./menu_action.hpp"
#include "./surface_creator.hpp"
#include "./crack_creator.hpp"
#include "./layerstack_control.hpp"

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
		using NoiseGenDlg = Ui::Dialog<SurfaceCreator>;
		using CrackGenDlg = Ui::Dialog<CrackCreator>;

	public:
		enum class ControlId : int
		{
			PaletteEd,
			LayerStackCtrl,
			Canvas
		};

		explicit AppWindow(Ui::Container& container):
		   m_canvas_size{512, 512},
		   m_current_color{0.5f, 0.5f, 0.5f, 1.0f},
		   m_painting{false},
		   m_paintmode{PaintMode::Draw},
		   m_keymask{0},
		   m_layerstack_ctrl{m_rows, m_canvas_size},
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
			m_layerstack_ctrl.eventHandler<ControlId::LayerStackCtrl>(*this);
			m_img_view.eventHandler<ControlId::Canvas>(*this);
			m_menu.eventHandler(*this);
		}


		template<AppAction>
		void onActivated(Ui::MenuItem&)
		{
		}

		template<FileAction>
		void onActivated(Ui::MenuItem&)
		{
		}

		template<LayerAction>
		void onActivated(Ui::MenuItem&)
		{
		}

		template<LayerActionNew>
		void onActivated(Ui::MenuItem&)
		{
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
		void onChanged(LayerStackControl&)
		{
			doRender();
			m_img_view.focus();
		}

		template<ControlId>
		void onChanged(PaletteEditor& pal)
		{
			m_current_color = pal.selectedPalette()[pal.selectedColorIndex()];
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
		Size2d m_canvas_size;
		Model::Pixel m_current_color;
		enum class PaintMode : int
		{
			Draw,
			Grab,
			Scale,
			Rotate
		};
		bool m_painting;
		PaintMode m_paintmode;
		static constexpr uint32_t KeymaskShift = 0x1;
		static constexpr uint32_t KeymaskCtrl = 0x2;
		uint32_t m_keymask;
		vec2_t m_paint_start_pos;
		LayerStackControl m_layerstack_ctrl;

		Ui::Box m_rows;
		Ui::MenuBuilder<MainMenuItem, MainMenuItemTraits> m_menu;
		Ui::Box m_selectors;
		Ui::LabeledInput<Ui::Combobox> m_layer_selector;
		Ui::LabeledInput<PaletteEditor> m_pal_editor;
		Ui::ImageView m_img_view;

		std::unique_ptr<NoiseGenDlg> m_surf_creator;
		Generators::CrackGenerator::Rng m_rng;
		std::unique_ptr<CrackGenDlg> m_crack_creator;


		void doRender()
		{
			Model::Image canvas{m_canvas_size};
			std::ranges::fill(canvas.pixels(), Model::Pixel{0.0f, 0.0f, 0.0f, 0.0f});
			render(m_layerstack_ctrl.layers(), canvas.pixels());
			m_layerstack_ctrl.outlineCurrentLayer(canvas.pixels());
			m_img_view.image(canvas);
		}
	};

	template<>
	inline void AppWindow::onMouseUp<AppWindow::ControlId::Canvas>(Ui::ImageView&, vec2_t, vec2_t, int)
	{
		m_painting = false;
	}

	template<>
	inline void AppWindow::onKeyDown<AppWindow::ControlId::Canvas>(Ui::ImageView& view, int scancode)
	{
		switch(scancode)
		{
			case 34: // G
				m_paintmode = PaintMode::Grab;
				break;
			case 31: // S
				m_paintmode = PaintMode::Scale;
				break;
			case 19: // R
				m_paintmode = PaintMode::Rotate;
				break;
			case 29: m_keymask |= KeymaskCtrl; break;
			case 42: m_keymask |= KeymaskShift; break;
			default: printf("%d\n", scancode);
		}
	}

	template<>
	inline void AppWindow::onKeyUp<AppWindow::ControlId::Canvas>(Ui::ImageView&, int scancode)
	{
		m_paintmode = PaintMode::Draw;
		switch(scancode)
		{
			case 29: m_keymask &= ~KeymaskCtrl; break;
			case 42: m_keymask &= ~KeymaskShift; break;
		}
	}

	template<>
	inline void AppWindow::onMouseDown<AppWindow::ControlId::Canvas>(Ui::ImageView& view,
	                                                                 vec2_t pos_window,
	                                                                 vec2_t pos_screen,
	                                                                 int button)
	{
		m_painting = true;
		auto const size = view.imageSize();
		m_paint_start_pos =
		   pos_window
		   - 0.5 * vec2_t{static_cast<double>(size.width()), static_cast<double>(size.height())};
		switch(m_paintmode)
		{
			case PaintMode::Draw:
			{
				auto const offset =
				   0.5 * vec2_t{static_cast<double>(size.width()), static_cast<double>(size.height())};
				m_layerstack_ctrl.paintCurrentLayer(pos_window - offset, 4.0, m_current_color);
				doRender();
			}
			break;

			default: break;
		}
	}
}

#endif