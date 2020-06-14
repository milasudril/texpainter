//@	{
//@	 "targets":[{"name":"image_surface.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"image_surface.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_IMAGESURFACE_HPP
#define TEXPAINTER_UI_IMAGESURFACE_HPP

#include "./container.hpp"

#include "geom/dimension.hpp"
#include "model/image.hpp"

#include <utility>

namespace Texpainter::Ui
{
	class ImageSurface
	{
		public:
 			explicit ImageSurface(Container& container);
			~ImageSurface();

			ImageSurface& operator=(ImageSurface&& obj) noexcept
			{
				std::swap(obj.m_impl, m_impl);
				return *this;
			}

			ImageSurface(ImageSurface&& obj) noexcept:m_impl(obj.m_impl)
			{ obj.m_impl=nullptr; }

			template<auto id, class EventHandler>
			ImageSurface& eventHandler(EventHandler& eh)
			{
				return eventHandler(&eh, {
					[](void* event_handler, ImageSurface& self, vec2_t pos_window, vec2_t pos_screen, int button)
					{
						auto& obj = *reinterpret_cast<EventHandler*>(event_handler);
						obj.template onMouseDown<id>(self, pos_window, pos_screen, button);
					}
				});
			}

			ImageSurface& image(Model::Image const& img);
			ImageSurface& update();

		private:
			class Impl;
			explicit ImageSurface(Impl& impl):m_impl(&impl){}
			Impl* m_impl;
			struct EventHandlerVtable
			{
				void (*m_on_mouse_down)(void* event_handler, ImageSurface& self, vec2_t pos_window, vec2_t pos_screen, int button);
			};
			ImageSurface& eventHandler(void* event_handler, EventHandlerVtable const& vtable);
	};
}

#endif