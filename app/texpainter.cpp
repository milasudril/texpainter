//@	{"targets":[{"name":"texpainter","type":"application", "autorun":0,"pkgconfig_libs":["gtk+-3.0"]}]}

#include "ui/window.hpp"
#include "ui/image_surface.hpp"
// #include "ui/box.hpp"

#include <gtk/gtk.h>

struct MyCallback
{
	template<int>
	void closing(Texpainter::Ui::Window&)
	{Texpainter::Ui::Window::terminateApp();}

	template<int>
	void onMouseDown(Texpainter::Ui::ImageSurface&, Texpainter::vec2_t pos_window, Texpainter::vec2_t pos_screen)
	{
		printf("(%.15g %.15g), (%.15g %.15g)\n", pos_window[0], pos_window[1], pos_screen[0], pos_screen[1]);
	}
};

int main(int argc, char* argv[])
{
	gtk_init(&argc, &argv);


	Texpainter::Ui::Window mainwin{"Texpainter"};
	mainwin.defaultSize(Texpainter::Geom::Dimension{}.width(800).height(500));

	MyCallback cb;

	Texpainter::Model::Image img{641, 480};

	Texpainter::Ui::ImageSurface imgview{mainwin};
	imgview.image(img).eventHandler<0>(cb);


//	Texpainter::Ui::Box box{mainwin, Texpainter::UiBox::Orientation::Vertical};

	mainwin.callback<0>(cb).show();
	gtk_main();
}