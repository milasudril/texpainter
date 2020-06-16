//@	{"targets":[{"name":"texpainter","type":"application", "pkgconfig_libs":["gtk+-3.0"]}]}

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
	void onMouseDown(Texpainter::Ui::PaletteView& view, size_t, int)
	{
	}

	using ColorPicker = Texpainter::Ui::Dialog<
	   Texpainter::Ui::ExtendedColorPicker<Texpainter::Ui::ColorPickerSidepanel>>;
	template<int>
	void onMouseUp(Texpainter::Ui::PaletteView&, size_t index, int button)
	{
		if(index < std::size(std::as_const(r_doc.get()).palette()))
		{
			switch(button)
			{
				case 3:
				{
					m_modified_pal_index = index;
					r_palview->highlightMode(index, Texpainter::Ui::PaletteView::HighlightMode::Write);
					m_color_picker = std::make_unique<ColorPicker>(
					   *r_mainwin, (std::string{"Select color number "} + std::to_string(index + 1)).c_str());
					m_color_picker->widget().value(std::as_const(r_doc.get()).palette()[index]);
					m_color_picker->eventHandler<0>(*this);
					m_color_picker->show();
				}
				break;
				case 1:
					r_palview
					   ->highlightMode(r_doc.get().selectedColorIndex(),
					                   Texpainter::Ui::PaletteView::HighlightMode::None)
					   .highlightMode(index, Texpainter::Ui::PaletteView::HighlightMode::Read);
					r_doc.get().selectedColorIndex(index);
					break;
			}
		}
	}

	template<int>
	void dismiss(ColorPicker&)
	{
		r_palview->highlightMode(m_modified_pal_index, Texpainter::Ui::PaletteView::HighlightMode::None)
		   .highlightMode(r_doc.get().selectedColorIndex(),
		                  Texpainter::Ui::PaletteView::HighlightMode::Read);
		m_color_picker.reset();
	}

	template<int>
	void confirmPositive(ColorPicker& picker)
	{
		auto color_new = picker.widget().sidepanel().targetIntensity()(picker.widget().value());
		r_doc.get().palette()[m_modified_pal_index] = color_new;
		r_palview->highlightMode(m_modified_pal_index, Texpainter::Ui::PaletteView::HighlightMode::None)
		   .highlightMode(r_doc.get().selectedColorIndex(),
		                  Texpainter::Ui::PaletteView::HighlightMode::Read);
		m_color_picker.reset();
	}

	template<int>
	void onMouseMove(Texpainter::Ui::PaletteView&, size_t)
	{
	}

	template<int>
	void onClicked(Texpainter::Ui::Button& button)
	{
		button.state(false);
	}

	std::reference_wrapper<Texpainter::Model::Document> r_doc;

	Texpainter::Ui::Window* r_mainwin;
	Texpainter::Ui::PaletteView* r_palview;
	std::unique_ptr<ColorPicker> m_color_picker;

	size_t m_modified_pal_index;
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
	box_outer.homogenous(false);
	cb.r_mainwin = &mainwin;

	Texpainter::Ui::PaletteView palview{box_outer};
	palview.palette(std::as_const(doc.palette())).eventHandler<0>(cb);
	cb.r_palview = &palview;

	box_outer.insertMode(
	   Texpainter::Ui::Box::InsertMode{4, Texpainter::Ui::Box::Expand | Texpainter::Ui::Box::Fill});
	Texpainter::Ui::ImageView imgview{box_outer};
	imgview.image(std::as_const(doc.image())).eventHandler<0>(cb);

	// Texpainter::Ui::Button btn{mainwin, "Hej"};
	// btn.eventHandler<0>(cb);


	//	Texpainter::Ui::Box box{mainwin, Texpainter::UiBox::Orientation::Vertical};

	mainwin.eventHandler<0>(cb).show();
	gtk_main();
}