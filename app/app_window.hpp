//@	{"targets":[{"name":"app_window.hpp","type":"include"}]}

#ifndef TEXPAINTER_APPWINDOW_HPP
#define TEXPAINTER_APPWINDOW_HPP

#include "./palette_editor.hpp"
#include "./menu_action.hpp"
#include "./surface_creator.hpp"
#include "./crack_creator.hpp"
#include "./layerstack_control.hpp"

#include "ui/box.hpp"
#include "ui/toolbar.hpp"
#include "ui/image_view.hpp"
#include "ui/labeled_input.hpp"
#include "ui/dialog.hpp"

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
		   m_columns{container, Ui::Box::Orientation::Horizontal},
		   m_toolbar{m_columns, Ui::Box::Orientation::Vertical},
		   m_tools_separator{m_columns},
		   m_rows{m_columns.insertMode(Ui::Box::InsertMode{0, Ui::Box::Fill | Ui::Box::Expand}),
		          Ui::Box::Orientation::Vertical},
		   m_pal_editor{m_rows, Ui::Box::Orientation::Horizontal, "Palettes: "},
		   m_pal_separator{m_rows},
		   m_layerstack_ctrl{m_rows, Ui::Box::Orientation::Horizontal, "Layers: ", m_canvas_size},
		   m_layeres_separator{m_rows},
		   m_img_view{m_rows.insertMode(Ui::Box::InsertMode{0, Ui::Box::Fill | Ui::Box::Expand})},
		   m_painting{false},
		   m_paintmode{PaintMode::Draw}
		{
			forEachEnumItem<MenuAction>([this](auto tag) {
				if constexpr(std::is_same_v<Ui::Button, typename MenuActionTraits<tag.value>::type>)
				{
					m_toolbar.get<tag.value>()
					   .label(MenuActionTraits<tag.value>::displayName())
					   .template eventHandler<tag.value>(*this);
				}
			});
			m_pal_editor.inputField().eventHandler<ControlId::PaletteEd>(*this);
			m_layerstack_ctrl.inputField().eventHandler<ControlId::LayerStackCtrl>(*this);
			m_img_view.eventHandler<ControlId::Canvas>(*this);
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

		template<MenuAction id>
		void onClicked(Ui::Button& btn)
		{
			btn.state(false);
		}

		template<MenuAction id>
		void dismiss(NoiseGenDlg& dlg)
		{
			m_surf_creator.reset();
			m_toolbar.get<id>().state(false);
		}

		template<MenuAction id>
		void confirmPositive(NoiseGenDlg& dlg)
		{
			m_toolbar.get<id>().state(false);
			//			m_img = m_surf_creator->widget().generate(Size2d{m_canvas_size});
			m_surf_creator.reset();
			//			m_img_view.image(m_img);
		}

		template<MenuAction id>
		void dismiss(CrackGenDlg& dlg)
		{
			m_crack_creator.reset();
			m_toolbar.get<id>().state(false);
		}

		template<MenuAction id>
		void confirmPositive(CrackGenDlg& dlg)
		{
			m_toolbar.get<id>().state(false);
			//			m_img = m_crack_creator->widget().generate(Size2d{m_canvas_size});
			m_crack_creator.reset();
			//			m_img_view.image(m_img);
		}

	private:
		Size2d m_canvas_size;
		Model::Pixel m_current_color;

		Ui::Box m_columns;
		Ui::Toolbar<MenuAction, MenuActionTraits> m_toolbar;
		Ui::Separator m_tools_separator;
		Ui::Box m_rows;
		Ui::LabeledInput<PaletteEditor> m_pal_editor;
		Ui::Separator m_pal_separator;
		Ui::LabeledInput<LayerStackControl> m_layerstack_ctrl;
		Ui::Separator m_layeres_separator;
		Ui::ImageView m_img_view;

		std::unique_ptr<NoiseGenDlg> m_surf_creator;
		Generators::CrackGenerator::Rng m_rng;
		std::unique_ptr<CrackGenDlg> m_crack_creator;


		void doRender()
		{
			Model::Image canvas{m_canvas_size};
			std::ranges::fill(canvas.pixels(), Model::Pixel{0.0f, 0.0f, 0.0f, 0.0f});
			render(m_layerstack_ctrl.inputField().layers(), canvas.pixels());
			m_layerstack_ctrl.inputField().outlineCurrentLayer(canvas.pixels());
			m_img_view.image(canvas);
		}

		bool m_painting;
		enum class PaintMode : int
		{
			Draw,
			Grab,
			Scale
		};
		PaintMode m_paintmode;
		vec2_t m_paint_start_pos;
	};

	template<>
	void AppWindow::onClicked<MenuAction::GenNoise>(Ui::Button& btn)
	{
		m_surf_creator = std::make_unique<NoiseGenDlg>(m_columns, "Generate noise");
		m_surf_creator->eventHandler<MenuAction::GenNoise>(*this);
	}

	template<>
	void AppWindow::onClicked<MenuAction::GenCracks>(Ui::Button& btn)
	{
		m_crack_creator = std::make_unique<CrackGenDlg>(m_columns, "Generate cracks", m_rng);
		m_crack_creator->eventHandler<MenuAction::GenCracks>(*this);
	}

	template<>
	void AppWindow::onMouseUp<AppWindow::ControlId::Canvas>(Ui::ImageView&, vec2_t, vec2_t, int)
	{
		m_painting = false;
	}

	template<>
	void AppWindow::onKeyDown<AppWindow::ControlId::Canvas>(Ui::ImageView& view, int scancode)
	{
		switch(scancode)
		{
			case 34: // G
				m_paintmode = PaintMode::Grab;
				break;
			case 31: // S
				m_paintmode = PaintMode::Scale;
				break;
			default: printf("%d\n", scancode);
		}
	}

	template<>
	void AppWindow::onKeyUp<AppWindow::ControlId::Canvas>(Ui::ImageView&, int)
	{
		m_paintmode = PaintMode::Draw;
	}

	template<>
	void AppWindow::onMouseDown<AppWindow::ControlId::Canvas>(Ui::ImageView& view,
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
				m_layerstack_ctrl.inputField().paintCurrentLayer(pos_window - offset, 16.0, m_current_color);
				doRender();
			}
			break;

			default: break;
		}
	}

	template<>
	void AppWindow::onMouseMove<AppWindow::ControlId::Canvas>(Ui::ImageView& view,
	                                                          vec2_t pos_window,
	                                                          vec2_t pos_screen)
	{
		if(!m_painting) { return; }

		auto const size = view.imageSize();
		auto const offset =
		   0.5 * vec2_t{static_cast<double>(size.width()), static_cast<double>(size.height())};
		auto const loc = pos_window - offset;

		switch(m_paintmode)
		{
			case PaintMode::Draw:
			{
				m_layerstack_ctrl.inputField().paintCurrentLayer(loc, 16.0, m_current_color);
				doRender();
			}
			break;

			case PaintMode::Grab:
			{
				m_layerstack_ctrl.inputField().moveCurrentLayer(loc);
				doRender();
			}
			break;

			case PaintMode::Scale:
			{
				auto const d = loc / m_paint_start_pos;
				m_layerstack_ctrl.inputField().scaleCurrentLayer(d);
				doRender();
			}
			break;
		}
	}
}

#endif