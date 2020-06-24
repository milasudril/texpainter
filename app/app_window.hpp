//@	{"targets":[{"name":"app_window.hpp","type":"include"}]}

#ifndef TEXPAINTER_APPWINDOW_HPP
#define TEXPAINTER_APPWINDOW_HPP

#include "./palette_editor.hpp"
#include "./menu_action.hpp"
#include "ui/box.hpp"
#include "ui/toolbar.hpp"
#include "ui/image_view.hpp"
#include "ui/labeled_input.hpp"

namespace Texpainter
{
	class AppWindow
	{
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

	private:
		Model::Image m_img;
		Ui::Box m_columns;
		Ui::Toolbar<MenuAction, MenuActionTraits> m_toolbar;
		Ui::Box m_rows;
		Ui::LabeledInput<PaletteEditor> m_pal_editor;
		Ui::ImageView m_img_view;
	};
}

#endif
