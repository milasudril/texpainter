//@	{
//@  "targets":[{"name":"filled_shape.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"filled_shape.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_FILLEDSHAPE_HPP
#define TEXPAINTER_UI_FILLEDSHAPE_HPP

#include "./container.hpp"
#include "./box.hpp"
#include "./toplevel_coordinates.hpp"
#include "./dispatch_event.hpp"

#include "pixel_store/pixel.hpp"

#include <utility>
#include <numbers>

namespace Texpainter::Ui
{
	class FilledShape
	{
	public:
		explicit FilledShape(Container& cnt,
		                     PixelStore::Pixel color = PixelStore::Pixel{0.5f, 0.5f, 0.5f, 1.0f},
		                     double radius           = 1.0 - 1.0 / std::numbers::phi,
		                     vec2_t location         = vec2_t{0.0, 0.0});

		~FilledShape();

		FilledShape& operator=(FilledShape&& obj) noexcept
		{
			std::swap(obj.m_impl, m_impl);
			return *this;
		}

		FilledShape(FilledShape&& obj) noexcept: m_impl{obj.m_impl} { obj.m_impl = nullptr; }

		template<auto id, class EventHandler>
		FilledShape& eventHandler(EventHandler& eh)
		{
			return eventHandler(&eh, [](void* event_handler, FilledShape& self) {
				auto& obj = *reinterpret_cast<EventHandler*>(event_handler);
				dispatchEvent(
				    [](EventHandler& eh, auto&&... args) {
					    eh.template onClicked<id>(std::forward<decltype(args)>(args)...);
				    },
				    obj,
				    self);
			});
		}

		ToplevelCoordinates location() const;

	protected:
		class Impl;
		Impl* m_impl;
		explicit FilledShape(Impl& impl): m_impl(&impl) {}
		using EventHandlerFunc = void (*)(void* event_handler, FilledShape& self);
		FilledShape& eventHandler(void* event_handler, EventHandlerFunc f);
	};
}

#endif
