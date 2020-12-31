//@	{
//@	 "targets":
//@	 	[{"name":"context.o","type":"object", "pkgconfig_libs":["gtk+-3.0"],
//@			"cxxoptions_local":{"cflags_extra":["Wno-deprecated-declarations"]}
//@		}]
//@	}

#include "./context.hpp"

#include <gtk/gtk.h>

Texpainter::Ui::Context::Context(): m_stop{false}, m_event_index{0}
{
	gtk_disable_setlocale();
	gtk_init(nullptr, nullptr);
}

Texpainter::Size2d Texpainter::Ui::Context::primaryWorkspaceSize() const
{
	GdkRectangle workarea{};
	gdk_monitor_get_workarea(gdk_display_get_primary_monitor(gdk_display_get_default()), &workarea);
	return Size2d{static_cast<uint32_t>(workarea.width), static_cast<uint32_t>(workarea.height)};
}

void Texpainter::Ui::Context::run()
{
	auto snooper = gtk_key_snooper_install(
	    [](GtkWidget*, GdkEventKey* event, gpointer obj) {
		    auto scancode = Scancode{static_cast<uint8_t>(event->hardware_keycode - 8)};

		    // Do not register AltLeft, because that would be stuck in case of ALT+TAB
		    if(scancode == Scancodes::AltLeft) { return FALSE; }

		    auto& self = *reinterpret_cast<Context*>(obj);

		    if(event->type == GDK_KEY_PRESS) { self.m_key_state.press(scancode); }
		    else
		    {
			    self.m_key_state.release(scancode);
		    }

		    return FALSE;
	    },
	    this);

	while(!m_stop)
	{
		while(!m_actions.empty())
		{
			auto& action = m_actions.top();
			if(action.first <= m_event_index)
			{
				action.second();
				m_actions.pop();
			}
			else
			{
				break;
			}
		}
		gtk_main_iteration();
		++m_event_index;
	}
	gtk_key_snooper_remove(snooper);
}

void Texpainter::Ui::Context::exit() { m_stop = true; }