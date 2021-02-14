//@	{"targets":[{"name":"texpainter","type":"application", "dependencies":[{"ref":"notify", "rel":"external"}]}]}

#include "./window_manager.hpp"
#include "ui/context.hpp"

#include <libnotify/notify.h>

namespace
{
	void notify(char const* msg)
	{
		notify_init("Texpainter");
		auto notification =
		    notify_notification_new("Failed to init texpainter", msg, "dialog-warning");
		notify_notification_show(notification, nullptr);
		g_object_unref(notification);
		notify_uninit();
	}
}

int main(int argc, char* argv[])
{
	try
	{
		auto& context = Texpainter::Ui::Context::get();
		Texpainter::App::WindowManager wm;

		if(argc > 1) { wm.loadDocument(argv[1]); }

		context.run();
		return 0;
	}
	catch(std::string const& msg)
	{
		notify(msg.c_str());
	}
	catch(std::exception const& e)
	{
		notify(e.what());
	}
	catch(char const* msg)
	{
		notify(msg);
	}

	return -1;
}