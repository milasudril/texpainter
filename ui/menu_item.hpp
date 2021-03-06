//@	{
//@	 "targets":[{"name":"menu_item.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"menu_item.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_MENUITEM_HPP
#define TEXPAINTER_UI_MENUITEM_HPP

#include "./container.hpp"
#include "./dispatch_event.hpp"

#include <utility>

namespace Texpainter::Ui
{
	class MenuItem: public Container
	{
	public:
		explicit MenuItem(Container& parent, char const* label = "");
		~MenuItem();

		MenuItem& operator=(MenuItem&& obj) noexcept
		{
			std::swap(obj.m_impl, m_impl);
			return *this;
		}

		MenuItem(MenuItem&& obj) noexcept: m_impl(obj.m_impl) { obj.m_impl = nullptr; }

		template<auto id, class EventHandler>
		MenuItem& eventHandler(EventHandler& eh)
		{
			return eventHandler(&eh, [](void* eh, MenuItem& self) {
				auto& obj = *reinterpret_cast<EventHandler*>(eh);
				dispatchEvent<id>(
				    [](EventHandler& eh, auto&&... args) {
					    eh.template onActivated<id>(std::forward<decltype(args)>(args)...);
				    },
				    obj,
				    self);
			});
		}

		MenuItem& label(char const* label);
		char const* label() const;

		MenuItem& add(void* handle) override;
		MenuItem& show() override;
		MenuItem& sensitive(bool val) override;
		MenuItem& killFocus() override { return *this; }

		void* toplevel() const override;


	protected:
		class Impl;
		explicit MenuItem(MenuItem::Impl& impl): m_impl(&impl) {}
		Impl* m_impl;

		using EventHandlerFunc = void (*)(void* event_handler, MenuItem& self);
		MenuItem& eventHandler(void* event_handler, EventHandlerFunc f);
	};
}

#endif
