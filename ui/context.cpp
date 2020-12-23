//@	{
//@	 "targets":
//@	 	[{"name":"context.o","type":"object", "pkgconfig_libs":["gtk+-3.0"],
//@			"cxxoptions_local":{"cflags_extra":["Wno-deprecated-declarations"]}
//@		}]
//@	}

#include "./context.hpp"

#include <gtk/gtk.h>

Texpainter::Ui::Context::Context()
{
	gtk_disable_setlocale();
	gtk_init(nullptr, nullptr);
}

void Texpainter::Ui::Context::run()
{
	auto snooper = gtk_key_snooper_install(
	    [](GtkWidget*, GdkEventKey* event, gpointer obj) {
		    auto scancode = Scancode{static_cast<uint8_t>(event->hardware_keycode - 8)};
		    auto& self    = *reinterpret_cast<Context*>(obj);

		    if(event->type == GDK_KEY_PRESS) { self.m_key_state.press(scancode); }
		    else
		    {
			    self.m_key_state.release(scancode);
		    }

		    return FALSE;
	    },
	    this);
	gtk_main();
	gtk_key_snooper_remove(snooper);
}

void Texpainter::Ui::Context::exit()
{
	gtk_main_quit();
}