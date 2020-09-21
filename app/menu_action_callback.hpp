//@	{
//@	"targets":[{"name":"menu_action_callback.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_APP_MENUACTIONCALLBACK_HPP
#define TEXPAINTER_APP_MENUACTIONCALLBACK_HPP

#include "utils/function_ref.hpp"

namespace Texpainter
{
	template<auto Action>
	class MenuActionCallback: public SimpleCallback
	{
	public:
		template<class T>
		explicit MenuActionCallback(T& obj) requires(
		    !std::same_as<std::decay_t<T>, MenuActionCallback>)
		    : SimpleCallback(obj, Tag<Action>{})
		{
		}
	};
}

#endif