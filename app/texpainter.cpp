//@	{"targets":[{"name":"texpainter","type":"application", "pkgconfig_libs":["gtk+-3.0"]}]}


#include "./app_window.hpp"
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

	T& r_app;
};

int main(int argc, char* argv[])
{
	gtk_disable_setlocale();
	gtk_init(&argc, &argv);

	Texpainter::Ui::Window mainwin{"Texpainter"};
	mainwin.resize(Texpainter::Size2d{800, 500});

#if 0
	Texpainter::Model::Layer test{Texpainter::Size2d{256, 256}};
	std::ranges::fill(test.content().pixels(), Texpainter::Model::Pixel{0.5f, 0.25f, 0.25f, 1.0f});

	Texpainter::Model::Image canvas{Texpainter::Size2d{512, 512}};
	 std::ranges::fill(canvas.pixels(), Texpainter::Model::Pixel{0.25f, 0.50f, 0.25f, 1.0f});

	 render(test.location(Texpainter::vec2_t{0.0, 0.0})
	           .rotation(Texpainter::Angle{-0.08333, Texpainter::Angle::Turns{}})
	           .scaleFactor(Texpainter::vec2_t{1.0, 0.5}),
	        canvas.pixels());

	 Texpainter::Ui::ImageView img_view{mainwin};
	 img_view.image(canvas).minSize(Texpainter::Size2d{512, 512});
#endif

	pcg64 rng;
	Texpainter::AppWindow app{mainwin, Texpainter::PolymorphicRng{rng}};

	MainwinEventHandler eh{app};
	mainwin.eventHandler<0>(eh).show();
	gtk_main();
}
