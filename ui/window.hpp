//@	{
//@	 "targets":[{"name":"window.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"window.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_WINDOW_HPP
#define TEXPAINTER_UI_WINDOW_HPP

#include "./container.hpp"
#include "./scancode.hpp"
#include "./dispatch_event.hpp"
#include "./screen_coordinates.hpp"

#include "utils/size_2d.hpp"

#include <utility>

namespace Texpainter::Ui
{
	class Window: public Container
	{
	public:
		explicit Window(const char* title, Container* owner = nullptr);
		~Window() override;

		Window& operator=(Window&& obj) noexcept
		{
			std::swap(obj.m_impl, m_impl);
			return *this;
		}

		Window(Window&& obj) noexcept: m_impl(obj.m_impl) { obj.m_impl = nullptr; }

		Window& add(void* handle) override;
		Window& show() override;
		Window& sensitive(bool val) override;
		Window& killFocus() override;

		void* toplevel() const override;

		const char* title() const noexcept;
		Window& title(const char* title_new);

		template<auto id, class EventHandler>
		Window& eventHandler(EventHandler& eh)
		{
			return eventHandler(
			    &eh,
			    {[](void* event_handler, Window& self) {
				     auto& obj = *reinterpret_cast<EventHandler*>(event_handler);
				     dispatchEvent<id>(
				         [](EventHandler& eh, auto&&... args) {
					         eh.template onClose<id>(std::forward<decltype(args)>(args)...);
				         },
				         obj,
				         self);
			     },
			     [](void* event_handler, Window& self, Scancode key) {
				     auto& obj = *reinterpret_cast<EventHandler*>(event_handler);
				     dispatchEvent<id>(
				         [](EventHandler& eh, auto&&... args) {
					         eh.template onKeyDown<id>(std::forward<decltype(args)>(args)...);
				         },
				         obj,
				         self,
				         key);
			     },
			     [](void* event_handler, Window& self, Scancode key) {
				     auto& obj = *reinterpret_cast<EventHandler*>(event_handler);
				     dispatchEvent<id>(
				         [](EventHandler& eh, auto&&... args) {
					         eh.template onKeyUp<id>(std::forward<decltype(args)>(args)...);
				         },
				         obj,
				         self,
				         key);
			     }});
		}

		Window& modal(bool state);

		Window& defaultSize(Size2d size);

		Window& resize(Size2d size);

		Window& move(ScreenCoordinates coords);

		Size2d size() const;

		ScreenCoordinates location() const;

		static void terminateApp();

		Window& shrinkToContent();

		bool minimized() const;

		Window& minimized(bool status);

		bool maximized() const;

		Window& maximized(bool status);

	protected:
		struct EventHandlerVtable
		{
			void (*on_close)(void* event_handler, Window& self);
			void (*on_key_down)(void* event_handler, Window& self, Scancode key);
			void (*on_key_up)(void* event_handler, Window& self, Scancode key);
		};
		Window& eventHandler(void* event_handler, EventHandlerVtable const& vtable);

		class Impl;
		Impl* m_impl;
		explicit Window(Impl& impl): m_impl(&impl) {}
	};
}

#endif