//@	{"targets":[{"name":"app_window.hpp","type":"include"}]}

#ifndef TEXPAINTER_APPWINDOW_HPP
#define TEXPAINTER_APPWINDOW_HPP

#include "./palette_editor.hpp"
#include "./menu_action.hpp"
#include "./surface_creator.hpp"
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

	public:
		explicit AppWindow(Ui::Container& container):
		   m_img{512, 512},
		   m_columns{container, Ui::Box::Orientation::Horizontal},
		   m_toolbar{m_columns, Ui::Box::Orientation::Vertical},
		   m_rows{m_columns.insertMode(Ui::Box::InsertMode{0, Ui::Box::Fill | Ui::Box::Expand}),
		          Ui::Box::Orientation::Vertical},
		   m_pal_editor{m_rows, Ui::Box::Orientation::Horizontal, "Palettes: "},
		   m_img_view{m_rows.insertMode(Ui::Box::InsertMode{0, Ui::Box::Fill | Ui::Box::Expand})}
		{
			forEachEnumItem<MenuAction>([this](auto tag) {
				m_toolbar.get<tag.value>()
				   .label(MenuActionTraits<tag.value>::displayName())
				   .template eventHandler<tag.value>(*this);
			});
			std::ranges::fill(m_img.pixels(), Model::Pixel{0.5f, 0.5f, 0.5f, 1.0f});
			m_img_view.image(m_img);
		}

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
			m_img = m_surf_creator->widget().generate(Size2d{1024, 1024});
			m_surf_creator.reset();
			m_img_view.image(m_img);
		}

	private:
		Model::Image m_img;
		Ui::Box m_columns;
		Ui::Toolbar<MenuAction, MenuActionTraits> m_toolbar;
		Ui::Box m_rows;
		Ui::LabeledInput<PaletteEditor> m_pal_editor;
		Ui::ImageView m_img_view;

		std::unique_ptr<NoiseGenDlg> m_surf_creator;
	};

	template<>
	void AppWindow::onClicked<MenuAction::GenNoise>(Ui::Button& btn)
	{
		m_surf_creator = std::make_unique<NoiseGenDlg>(m_columns, "Generate noise");
		m_surf_creator->eventHandler<MenuAction::GenNoise>(*this);
	}
}

#endif
