//@	{
//@	 "targets":[{"name":"checkable_menu_item.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"checkable_menu_item.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_CHECKABLEMENUITEM_HPP
#define TEXPAINTER_UI_CHECKABLEMENUITEM_HPP

#include "./container.hpp"
#include "./dispatch_event.hpp"

#include <utility>

namespace Texpainter::Ui
{
	class CheckableMenuItem: public Container
	{
	public:
		explicit CheckableMenuItem(Container& parent, char const* label = "");
		~CheckableMenuItem();

		CheckableMenuItem& operator=(CheckableMenuItem&& obj) noexcept
		{
			std::swap(obj.m_impl, m_impl);
			return *this;
		}

		CheckableMenuItem(CheckableMenuItem&& obj) noexcept: m_impl(obj.m_impl)
		{
			obj.m_impl = nullptr;
		}

		template<auto id, class EventHandler>
		CheckableMenuItem& eventHandler(EventHandler& eh)
		{
			return eventHandler(&eh, [](void* eh, CheckableMenuItem& self) {
				auto& obj = *reinterpret_cast<EventHandler*>(eh);
				dispatchEvent<id>(
				    [](EventHandler& eh, auto&&... args) {
					    eh.template onActivated<id>(std::forward<decltype(args)>(args)...);
				    },
				    obj,
				    self);
			});
		}

		CheckableMenuItem& label(char const* label);
		CheckableMenuItem& status(bool val);
		bool status() const;
		CheckableMenuItem& toggle() { return status(!status()); }

		CheckableMenuItem& add(void* handle) override;
		CheckableMenuItem& show() override;
		CheckableMenuItem& sensitive(bool val) override;
		CheckableMenuItem& killFocus() override { return *this; }

		void* toplevel() const override;


	protected:
		class Impl;
		explicit CheckableMenuItem(CheckableMenuItem::Impl& impl): m_impl(&impl) {}
		Impl* m_impl;

		using EventHandlerFunc = void (*)(void* event_handler, CheckableMenuItem& self);
		CheckableMenuItem& eventHandler(void* event_handler, EventHandlerFunc f);
	};
}

#endif
