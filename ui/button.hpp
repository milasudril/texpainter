//@	{
//@	 "targets":[{"name":"button.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"button.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_BUTTON_HPP
#define TEXPAINTER_UI_BUTTON_HPP

#include "./container.hpp"
#include "./dispatch_event.hpp"

#include <utility>
#include <exception>

namespace Texpainter::Ui
{
	class Button
	{
	public:
		explicit Button(Container& container, const char* label = "");
		~Button();

		Button& operator=(Button&& obj) noexcept
		{
			std::swap(obj.m_impl, m_impl);
			return *this;
		}

		Button(Button&& obj) noexcept: m_impl(obj.m_impl) { obj.m_impl = nullptr; }

		template<auto id, class EventHandler>
		Button& eventHandler(EventHandler& eh)
		{
			return eventHandler(&eh, [](void* event_handler, Button& self) {
				auto& obj = *reinterpret_cast<EventHandler*>(event_handler);
				dispatchEvent<id>(
				    [](EventHandler& eh, auto&&... args) {
					    eh.template onClicked<id>(std::forward<decltype(args)>(args)...);
				    },
				    obj,
				    self);
			});
		}

		const char* label() const noexcept;

		Button& label(const char* text);

		Button& state(bool s) noexcept;

		Button& toggle() { return state(!state()); }

		bool state() const noexcept;

		Button& focus() noexcept;

		Button& small(bool status) noexcept;

	private:
		class Impl;
		explicit Button(Impl& impl): m_impl{&impl} {}
		Impl* m_impl;
		using EventHandlerFunc = void (*)(void* event_handler, Button& self);
		Button& eventHandler(void* event_handler, EventHandlerFunc f);
	};
}

#endif