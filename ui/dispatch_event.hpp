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
	template<class Callback, class EventHandler, class... Args>
	void dispatchEvent(Callback&& cb, EventHandler& eh, Args&&... args)
	{
		try
		{
			std::invoke(cb, eh, std::forward<Args>(args)...);
		}
		catch(std::exception const& exception)
		{
			eh.handleException(exception.what());
		}
		catch(std::string const& str)
		{
			eh.handleException(str.c_str());
		}
		catch(char const* msg)
		{
			eh.handleException(msg);
		}
		catch(...)
		{
			fprintf(stderr, "Fatal error: Unknown exception thrown\n");
			std::terminate();
		}
	}
}

#endif