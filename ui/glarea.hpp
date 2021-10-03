//@	{
//@	 "targets":[{"name":"glarea.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"glarea.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_GLAREA_HPP
#define TEXPAINTER_UI_GLAREA_HPP

#include <utility>

namespace Texpainter::Ui
{
	class Container;

	class GLArea
	{
	public:
		explicit GLArea(Container& cnt);
		~GLArea();

		GLArea& operator=(GLArea&& obj) noexcept
		{
			std::swap(obj.m_impl, m_impl);
			return *this;
		}

		GLArea(GLArea&& obj) noexcept: m_impl(obj.m_impl) { obj.m_impl = nullptr; }

		GLArea& minHeight(int h);

		GLArea& minWidth(int w);

		GLArea& minSize(int w, int h);

		template<auto id, class EventHandler>
		GLArea& eventHandler(EventHandler& eh)
		{
			return eventHandler(&eh,
			                    {[](void* event_handler, GLArea& self) {
				                     auto& obj = *static_cast<EventHandler*>(event_handler);
				                     obj.template render<id>(self);
			                     },
			                     [](void* event_handler, GLArea& self, int width, int height) {
				                     auto& obj = *static_cast<EventHandler*>(event_handler);
				                     obj.template resize<id>(self, width, height);
			                     },
			                     [](void* event_handler, GLArea& self) {
				                     auto& obj = *reinterpret_cast<EventHandler*>(event_handler);
				                     obj.template realize<id>(self);
			                     }});
		}
		GLArea& glActivate();

		GLArea& versionRequest(int major, int minor);

		GLArea& redraw() noexcept;

	private:
		class Impl;
		Impl* m_impl;

		GLArea(Impl& impl): m_impl(&impl) {}

		struct EventHandlerVtable
		{
			void (*render)(void* cb_obj, GLArea& source);
			void (*resize)(void* cb_obj, GLArea& source, int width, int height);
			void (*realize)(void* cb_obj, GLArea& source);
		};

		GLArea& eventHandler(void* cb_obj, const EventHandlerVtable& vt);
	};
}

#endif  // UIXX_IMAGEVIEW_HPP
