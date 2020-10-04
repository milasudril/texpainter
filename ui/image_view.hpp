//@	{
//@	 "targets":[{"name":"image_view.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"image_view.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_IMAGEVIEW_HPP
#define TEXPAINTER_UI_IMAGEVIEW_HPP

#include "./container.hpp"
#include "./dispatch_event.hpp"

#include "pixel_store/image.hpp"

#include <utility>

namespace Texpainter::Ui
{
	class ImageView
	{
	public:
		explicit ImageView(Container& container);
		~ImageView();

		ImageView& operator=(ImageView&& obj) noexcept
		{
			std::swap(obj.m_impl, m_impl);
			return *this;
		}

		ImageView(ImageView&& obj) noexcept: m_impl(obj.m_impl) { obj.m_impl = nullptr; }

		template<auto id, class EventHandler>
		ImageView& eventHandler(EventHandler& eh)
		{
			return eventHandler(
			    &eh,
			    {[](void* event_handler,
			        ImageView& self,
			        vec2_t loc_window,
			        vec2_t loc_screen,
			        int button) {
				     auto& obj = *reinterpret_cast<EventHandler*>(event_handler);
				     obj.template onMouseDown<id>(self, loc_window, loc_screen, button);
			     },
			     [](void* event_handler,
			        ImageView& self,
			        vec2_t loc_window,
			        vec2_t loc_screen,
			        int button) {
				     auto& obj = *reinterpret_cast<EventHandler*>(event_handler);
				     dispatchEvent(
				         [](EventHandler& eh, auto&&... args) {
					         eh.template onMouseUp<id>(std::forward<decltype(args)>(args)...);
				         },
				         obj,
				         self,
				         loc_window,
				         loc_screen,
				         button);
			     },
			     [](void* event_handler, ImageView& self, vec2_t loc_window, vec2_t loc_screen) {
				     auto& obj = *reinterpret_cast<EventHandler*>(event_handler);
				     obj.template onMouseMove<id>(self, loc_window, loc_screen);
			     }});
		}

		ImageView& image(PixelStore::Image const& img);

		ImageView& clear();

		ImageView& minSize(Size2d size);

		Size2d imageSize() const noexcept;

		ImageView& focus();

		ImageView& alwaysEmitMouseEvents(bool status);

	private:
		class Impl;
		explicit ImageView(Impl& impl): m_impl(&impl) {}
		Impl* m_impl;
		struct EventHandlerVtable
		{
			void (*m_on_mouse_down)(void* event_handler,
			                        ImageView& self,
			                        vec2_t loc_window,
			                        vec2_t loc_screen,
			                        int button);
			void (*m_on_mouse_up)(void* event_handler,
			                      ImageView& self,
			                      vec2_t loc_window,
			                      vec2_t loc_screen,
			                      int button);
			void (*m_on_mouse_move)(void* event_handler,
			                        ImageView& self,
			                        vec2_t loc_window,
			                        vec2_t loc_screen);
		};
		ImageView& eventHandler(void* event_handler, EventHandlerVtable const& vtable);
	};
}

#endif