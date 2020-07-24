//@	{"targets":[{"name":"container.hpp","type":"include"}]}

#ifndef TEXPAINTER_UI_CONTAINER_HPP
#define TEXPAINTER_UI_CONTAINER_HPP

#include <gtk/gtk.h>

namespace Texpainter::Ui
{
	class Container
	{
	public:
		virtual ~Container()                   = default;
		virtual Container& add(void* handle)   = 0;
		virtual Container& show()              = 0;
		virtual Container& sensitive(bool val) = 0;
		Container& killFocus()
		{
			gtk_window_set_focus(GTK_WINDOW(toplevel()), NULL);
			return *this;
		}

		virtual void* toplevel() const         = 0;

	};
}

#endif
