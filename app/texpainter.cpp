//@	{"targets":[{"name":"texpainter","type":"application", "pkgconfig_libs":["gtk+-3.0"]}]}


#include "./app_window.hpp"

#include "model/layer_stack.hpp"

#include <gtk/gtk.h>

struct MainwinEventHandler
{
	template<int>
	void onClose(Texpainter::Ui::Window&)
	{
		Texpainter::Ui::Window::terminateApp();
	}

	template<int>
	void onKeyDown(Texpainter::Ui::Window&, int scancode)
	{
	}

	template<int>
	void onKeyUp(Texpainter::Ui::Window&, int scancode)
	{
	}
};

int main(int argc, char* argv[])
{
	gtk_disable_setlocale();
	gtk_init(&argc, &argv);

	Texpainter::Ui::Window mainwin{"Texpainter"};

	Texpainter::Model::Layer test{Texpainter::Size2d{256, 256}};
	std::ranges::fill(test.content().pixels(), Texpainter::Model::Pixel{0.5f, 0.25f, 0.25f, 1.0f});

/*	Texpainter::Model::Image canvas{Texpainter::Size2d{512, 512}};
	std::ranges::fill(canvas.pixels(), Texpainter::Model::Pixel{0.25f, 0.50f, 0.25f, 1.0f});

	render(test.location(Texpainter::vec2_t{0.0, 0.0})
	          .rotation(Texpainter::Angle{-0.08333, Texpainter::Angle::Turns{}})
	          .scaleFactor(Texpainter::vec2_t{1.0, 0.5}),
	       canvas.pixels());

	Texpainter::Ui::ImageView img_view{mainwin};
	img_view.image(canvas).minSize(Texpainter::Size2d{512, 512});*/


	Texpainter::AppWindow app{mainwin};

	MainwinEventHandler eh;
	mainwin.eventHandler<0>(eh).show();
	gtk_main();
}
