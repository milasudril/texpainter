//@	{
//@	 "targets":[{"name":"button.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"button.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_BUTTON_HPP
#define TEXPAINTER_BUTTON_HPP

#include "./container.hpp"

#include <utility>

namespace Texpainter::Ui
{
	class Button
	{
	public:
		explicit Button(Container& container, const char* label);
		~Button();

		Button& operator=(Button&& obj) noexcept
		{
			std::swap(obj.m_impl, m_impl);
			return *this;
		}

		Button(Button&& obj) noexcept: m_impl(obj.m_impl)
		{
			obj.m_impl = nullptr;
		}

		template<auto id, class EventHandler>
		Button& eventHandler(EventHandler& eh)
		{
			return eventHandler(&eh, [](void* event_handler, Button& self) {
				auto& obj = *reinterpret_cast<EventHandler*>(event_handler);
				obj.template onClicked<id>(self);
			});
		}

		const char* label() const noexcept;

		Button& label(const char* text);

		Button& state(bool s) noexcept;

		bool state() const noexcept;

		Button& focus() noexcept;

	private:
		class Impl;
		explicit Button(Impl& impl): m_impl(&impl)
		{
		}
		Impl* m_impl;
		using EventHandlerFunc = void (*)(void* event_handler, Button& self);
		Button& eventHandler(void* event_handler, EventHandlerFunc f);
	};
}

#endif
