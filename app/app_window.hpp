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
		   m_button_mask{0}
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
		}

		template<ControlId>
		void onChanged(PaletteEditor& pal)
		{
			m_current_color = pal.selectedPalette()[pal.selectedColorIndex()];
		}

		template<ControlId>
		void onMouseUp(Ui::ImageView& view, vec2_t pos_window, vec2_t pos_screen, int button);

		template<ControlId>
		void onMouseDown(Ui::ImageView& view, vec2_t pos_window, vec2_t pos_screen, int button);

		template<ControlId>
		void onMouseMove(Ui::ImageView& view, vec2_t pos_window, vec2_t pos_screen);

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

		uint32_t m_button_mask;

		void doRender()
		{
			Model::Image canvas{m_canvas_size};
			std::ranges::fill(canvas.pixels(), Model::Pixel{0.0f, 0.0f, 0.0f, 0.0f});
			render(m_layerstack_ctrl.inputField().layers(), canvas.pixels());
			m_img_view.image(canvas);
		}
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
	void AppWindow::onMouseUp<AppWindow::ControlId::Canvas>(Ui::ImageView&, vec2_t, vec2_t, int button)
	{
		m_button_mask &= ~(1 << button);
	}

	template<>
	void AppWindow::onMouseDown<AppWindow::ControlId::Canvas>(Ui::ImageView& view,
	                                                          vec2_t pos_window,
	                                                          vec2_t pos_screen,
	                                                          int button)
	{
		m_button_mask |= (1 << button);
		if(m_button_mask == 2)
		{
			auto const size = view.imageSize();
			auto const offset =
			   0.5 * vec2_t{static_cast<double>(size.width()), static_cast<double>(size.height())};
			m_layerstack_ctrl.inputField().paintCurrentLayer(pos_window - offset, 16.0, m_current_color);
			doRender();
		}
	}

	template<>
	void AppWindow::onMouseMove<AppWindow::ControlId::Canvas>(Ui::ImageView& view,
	                                                          vec2_t pos_window,
	                                                          vec2_t pos_screen)
	{
		if(m_button_mask == 2)
		{
			auto const size = view.imageSize();
			auto const offset =
			   0.5 * vec2_t{static_cast<double>(size.width()), static_cast<double>(size.height())};
			m_layerstack_ctrl.inputField().paintCurrentLayer(pos_window - offset, 16.0, m_current_color);
			doRender();
		}
	}
}

#endif