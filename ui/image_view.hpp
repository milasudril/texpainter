//@	{
//@	 "targets":[{"name":"image_view.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"image_view.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_IMAGEVIEW_HPP
#define TEXPAINTER_UI_IMAGEVIEW_HPP

#include "./container.hpp"
#include "model/image.hpp"

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

		ImageView(ImageView&& obj) noexcept: m_impl(obj.m_impl)
		{
			obj.m_impl = nullptr;
		}

		template<auto id, class EventHandler>
		ImageView& eventHandler(EventHandler& eh)
		{
			return eventHandler(
			   &eh,
			   {[](void* event_handler, ImageView& self, vec2_t pos_window, vec2_t pos_screen, int button) {
				    auto& obj = *reinterpret_cast<EventHandler*>(event_handler);
				    obj.template onMouseDown<id>(self, pos_window, pos_screen, button);
			    },
			    [](void* event_handler, ImageView& self, vec2_t pos_window, vec2_t pos_screen, int button) {
				    auto& obj = *reinterpret_cast<EventHandler*>(event_handler);
				    obj.template onMouseUp<id>(self, pos_window, pos_screen, button);
			    },
			    [](void* event_handler, ImageView& self, vec2_t pos_window, vec2_t pos_screen) {
				    auto& obj = *reinterpret_cast<EventHandler*>(event_handler);
				    obj.template onMouseMove<id>(self, pos_window, pos_screen);
			    },
			    [](void* event_handler, ImageView& self, int scancode) {
				    auto& obj = *reinterpret_cast<EventHandler*>(event_handler);
				    obj.template onKeyDown<id>(self, scancode);
			    },
			    [](void* event_handler, ImageView& self, int scancode) {
				    auto& obj = *reinterpret_cast<EventHandler*>(event_handler);
				    obj.template onKeyUp<id>(self, scancode);
			    }});
		}

		ImageView& image(Model::Image const& img);

		ImageView& minSize(Size2d size);

		Size2d imageSize() const noexcept;

		ImageView& focus();

	private:
		class Impl;
		explicit ImageView(Impl& impl): m_impl(&impl)
		{
		}
		Impl* m_impl;
		struct EventHandlerVtable
		{
			void (*m_on_mouse_down)(
			   void* event_handler, ImageView& self, vec2_t pos_window, vec2_t pos_screen, int button);
			void (*m_on_mouse_up)(
			   void* event_handler, ImageView& self, vec2_t pos_window, vec2_t pos_screen, int button);
			void (*m_on_mouse_move)(void* event_handler,
			                        ImageView& self,
			                        vec2_t pos_window,
			                        vec2_t pos_screen);
			void (*on_key_down)(void* event_handler, ImageView& self, int scancode);
			void (*on_key_up)(void* event_handler, ImageView& self, int scancode);
		};
		ImageView& eventHandler(void* event_handler, EventHandlerVtable const& vtable);
	};
}

#endif