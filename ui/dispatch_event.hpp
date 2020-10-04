//@	{
//@	 "targets":[{"name":"dispatch_event.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UI_DISPATCHEVENT_HPP
#define TEXPAINTER_UI_DISPATCHEVENT_HPP

#include <functional>
#include <string>
#include <exception>

namespace Texpainter::Ui
{
	template<auto id, class Callback, class Source, class EventHandler, class... Args>
	void dispatchEvent(Callback&& cb, EventHandler& eh, Source& src, Args&&... args)
	{
		try
		{
			std::invoke(cb, eh, src, std::forward<Args>(args)...);
		}
		catch(std::exception const& exception)
		{
			eh.template handleException<id>(exception.what(), src);
		}
		catch(std::string const& msg)
		{
			eh.template handleException<id>(msg.c_str(), src);
		}
		catch(char const* msg)
		{
			eh.template handleException<id>(msg, src);
		}
		catch(...)
		{
			fprintf(stderr, "Fatal error: Unknown exception thrown\n");
			std::terminate();
		}
	}
}

#endif