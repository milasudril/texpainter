//@	{"targets":[{"name":"texpainter","type":"application", "pkgconfig_libs":["gtk+-3.0"]}]}


#include "./app_window.hpp"

#include "utils/default_rng.hpp"

#include "pcg-cpp/include/pcg_random.hpp"

#include <gtk/gtk.h>

template<class T>
struct MainwinEventHandler
{
	template<int>
	void onClose(Texpainter::Ui::Window&)
	{
		Texpainter::Ui::Window::terminateApp();
	}

	template<int>
	void onKeyDown(Texpainter::Ui::Window&, Texpainter::Ui::Scancode key)
	{
		r_app.onKeyDown(key);
	}

	template<int>
	void onKeyUp(Texpainter::Ui::Window&, Texpainter::Ui::Scancode key)
	{
		r_app.onKeyUp(key);
	}

	void handleException(char const* msg) { r_app.handleException(msg); }

	T& r_app;
};

int main(int argc, char* argv[])
{
	gtk_disable_setlocale();
	gtk_init(&argc, &argv);

	Texpainter::Ui::Window mainwin{"Texpainter"};
	mainwin.resize(Texpainter::Size2d{800, 500});

	pcg64 rng;
	Texpainter::DefaultRng::engine(rng);

	Texpainter::AppWindow app{mainwin, Texpainter::DefaultRng::engine()};

	MainwinEventHandler eh{app};
	mainwin.eventHandler<0>(eh).show();
	gtk_main();
}
