//@	{"targets":[{"name":"slask2","type":"application", "pkgconfig_libs":["gtk+-3.0"]}]}

#include "utils/vec_t.hpp"

#include "ui/window.hpp"
#include "ui/widget_canvas.hpp"
#include "ui/label.hpp"
#include "ui/color_picker.hpp"

#include "pcg-cpp/include/pcg_random.hpp"

#include <gtk/gtk.h>


int main(int argc, char* argv[])
{
	gtk_disable_setlocale();
	gtk_init(&argc, &argv);

	Texpainter::Ui::Window mainwin{"Foo"};
	mainwin.resize(Texpainter::Size2d{800, 500});

	Texpainter::Ui::WidgetCanvas canvas{mainwin};

	pcg64 rng;
	auto widget = canvas.insert<Texpainter::Ui::ColorPicker>(
	    Texpainter::vec2_t{200, 200},
	    Texpainter::PolymorphicRng{rng},
	    "Foobar",
	    std::span<Texpainter::PixelStore::Pixel const>{});

	mainwin.show();

	gtk_main();

	return 0;
}