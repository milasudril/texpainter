//@	{"targets":[{"name":"texpainter","type":"application", "autorun":0,"pkgconfig_libs":["gtk+-3.0"]}]}

#include "ui/window.hpp"
// #include "ui/box.hpp"

#include <gtk/gtk.h>

struct MyCallback
{
	template<int>
	void closing(Texpainter::Ui::Window&)
	{Texpainter::Ui::Window::terminateApp();}
};

int main(int argc, char* argv[])
{
	gtk_init(&argc, &argv);


	Texpainter::Ui::Window mainwin{"Texpainter"};
	mainwin.defaultSize(Texpainter::Geom::Dimension{}.width(800).height(500));

//	Texpainter::Ui::Box box{mainwin, Texpainter::UiBox::Orientation::Vertical};

	MyCallback cb;
	mainwin.callback<0>(cb);
	mainwin.show();
	gtk_main();
}