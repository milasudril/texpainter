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
#include "generators/generators.hpp"
#include "generators/fourier_transform.hpp"
#include "generators/fourier_transform.hpp"

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
			auto& img = r_doc.get().image();
			img.get(static_cast<uint32_t>(pos_window[0]) % img.width(),
			        static_cast<uint32_t>(pos_window[1]) % img.height()) = selectedColor(r_doc);
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
			auto& img = r_doc.get().image();
			img.get(static_cast<uint32_t>(pos_window[0]) % img.width(),
			        static_cast<uint32_t>(pos_window[1]) % img.height()) = selectedColor(r_doc);
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

	Texpainter::Model::BasicImage<float> img{512, 512};

	std::mt19937 rng{};
	std::ranges::generate(img.pixels(), [&rng, U = std::uniform_real_distribution{0.0f, 1.0f}]() mutable{
		return U(rng);
	});



	for(int k = 0; k < 1; ++k)
	{
		if(k != 0)
			{
			std::ranges::for_each(img.pixels(), [&rng, U = std::uniform_real_distribution{0.0f, 1.0f}](auto& val) mutable{
				val += U(rng);
			});
		}

		for(uint32_t row = 1; row < img.height(); ++row)
		{
			for(uint32_t col = 1; col < img.width(); ++col)
			{
				constexpr auto a = -0.75f;
				img(col, row) = (img(col, row) + img(col, row - 1) + img(col - 1, row))/(3.0f + a);
			}
		}

	}

	auto pixels = img.pixels();
	auto res = std::ranges::minmax_element(pixels);
	printf("%.7g %.7g %.7g\n", *res.min, *res.max, *res.max - *res.min);

	auto& img_disp = doc.image();

	std::ranges::transform(
	   img.pixels(),
	   img_disp.pixels().begin(),
	   [min = *res.min, max = *res.max](auto val) {
			Texpainter::Model::Pixel offset{-min, -min, -min, 0.0f};
			Texpainter::Model::Pixel factor{max - min, max -min, max - min, 1.0f};
		   return (Texpainter::Model::Pixel{val,val,val} - offset)/factor;
	   });

	Texpainter::Ui::Window mainwin{"Texpainter"};
	mainwin.defaultSize(Texpainter::Geom::Dimension{}.width(800).height(600));
	Texpainter::Ui::Box box_outer{mainwin, Texpainter::Ui::Box::Orientation::Vertical};

	Texpainter::Ui::LabeledInput<Texpainter::PaletteEditor> pal_editor{
	   box_outer, Texpainter::Ui::Box::Orientation::Horizontal, "Palettes: "};
	pal_editor.inputField().eventHandler<0>(cb);

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