//@	{"targets":[{"name":"texpainter","type":"application", "dependencies":[{"ref":"notify", "rel":"external"}]}]}

#include "./window_manager.hpp"

#include "ui/context.hpp"
#include "log/logger.hpp"

#include <libnotify/notify.h>

namespace
{
	class NotifyLog
	{
	public:
		NotifyLog(char const* name) { notify_init(name); }

		~NotifyLog() { notify_uninit(); }
	};

	constexpr char const* msg_icon(Texpainter::Logger::MessageType type)
	{
		switch(type)
		{
			case Texpainter::Logger::MessageType::Info: return "dialog-information";
			case Texpainter::Logger::MessageType::Warning: return "dialog-waring";
			case Texpainter::Logger::MessageType::Error: return "dialog-error";

			default: __builtin_unreachable();
		}
	}

	void write(NotifyLog&, Texpainter::Logger::MessageType type, std::string&& msg)
	{
		auto notification = notify_notification_new("Texpainter", msg.c_str(), msg_icon(type));
		notify_notification_show(notification, nullptr);
		g_object_unref(notification);
	}
}

int main(int argc, char* argv[])
{
	NotifyLog notify{"Texpainter"};
	Texpainter::Logger::logger(notify);
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
		log(Texpainter::Logger::MessageType::Error, msg);
	}
	catch(std::exception const& e)
	{
		log(Texpainter::Logger::MessageType::Error, e.what());
	}
	catch(char const* msg)
	{
		log(Texpainter::Logger::MessageType::Error, msg);
	}

	return -1;
}