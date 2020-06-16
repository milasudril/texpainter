//@	{"targets":[{"name":"texpainter","type":"application", "pkgconfig_libs":["gtk+-3.0"]}]}

#include "./palette_editor.hpp"

#include "model/document.hpp"
#include "ui/window.hpp"
#include "ui/image_view.hpp"
#include "ui/palette_view.hpp"
#include "ui/extended_color_picker.hpp"
#include "ui/box.hpp"
#include "ui/color_picker_sidepanel.hpp"
#include "ui/dialog.hpp"

#include <gtk/gtk.h>

#include <functional>
#include <string>

struct MyCallback
{
	template<int>
	void onClose(Texpainter::Ui::Window&)
	{
		Texpainter::Ui::Window::terminateApp();
	}

	template<int>
	void onKeyDown(Texpainter::Ui::Window&, int scancode)
	{
		printf("(%d ", scancode);
		fflush(stdout);
	}

	template<int>
	void onKeyUp(Texpainter::Ui::Window&, int scancode)
	{
		printf("%d) ", scancode);
		fflush(stdout);
	}

	template<int>
	void onMouseDown(Texpainter::Ui::ImageView& event_source,
	                 Texpainter::vec2_t pos_window,
	                 Texpainter::vec2_t pos_screen,
	                 int button)
	{
		m_button_mask |= 1 << button;
		if(m_button_mask & (1 << 1))
		{
			r_doc.get().image().get(pos_window[0], pos_window[1]) = selectedColor(r_doc);
			event_source.update();
		}
	}

	template<int>
	void onMouseUp(Texpainter::Ui::ImageView& event_source,
	               Texpainter::vec2_t pos_window,
	               Texpainter::vec2_t,
	               int button)
	{
		m_button_mask &= ~(1 << button);
	}

	template<int>
	void onMouseMove(Texpainter::Ui::ImageView& event_source,
	                 Texpainter::vec2_t pos_window,
	                 Texpainter::vec2_t)
	{
		if(m_button_mask & (1 << 1))
		{
			r_doc.get().image().get(pos_window[0], pos_window[1]) = selectedColor(r_doc);
			event_source.update();
		}
	}

	template<int>
	void onClicked(Texpainter::Ui::Button& button)
	{
		button.state(false);
	}

	template<int>
	void onChanged(Texpainter::PaletteEditor& pal_editor)
	{
		r_doc.get().palette() = pal_editor.selectedPalette();
		r_doc.get().selectedColorIndex(pal_editor.selectedColorIndex());
	}


	std::reference_wrapper<Texpainter::Model::Document> r_doc;
	Texpainter::Ui::Window* r_mainwin;
	uint32_t m_button_mask{};
};

int main(int argc, char* argv[])
{
	gtk_disable_setlocale();
	gtk_init(&argc, &argv);

	Texpainter::Model::Document doc;

	MyCallback cb{doc};

	Texpainter::Ui::Window mainwin{"Texpainter"};
	mainwin.defaultSize(Texpainter::Geom::Dimension{}.width(800).height(500));
	Texpainter::Ui::Box box_outer{mainwin, Texpainter::Ui::Box::Orientation::Vertical};
	Texpainter::PaletteEditor pal_editor{box_outer};
	pal_editor.eventHandler<0>(cb);

	box_outer.homogenous(false);
	cb.r_mainwin = &mainwin;


	box_outer.insertMode(
	   Texpainter::Ui::Box::InsertMode{4, Texpainter::Ui::Box::Expand | Texpainter::Ui::Box::Fill});
	Texpainter::Ui::ImageView imgview{box_outer};
	imgview.image(std::as_const(doc.image())).eventHandler<0>(cb);


	box_outer.insertMode(Texpainter::Ui::Box::InsertMode{4, 0});

	// Texpainter::Ui::Button btn{mainwin, "Hej"};
	// btn.eventHandler<0>(cb);


	//	Texpainter::Ui::Box box{mainwin, Texpainter::UiBox::Orientation::Vertical};

	mainwin.eventHandler<0>(cb).show();
	gtk_main();
}