//@	{
//@	"targets":[{"name":"listbox.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"listbox.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_LISTBOX_HPP
#define TEXPAINTER_UI_LISTBOX_HPP

#include "./container.hpp"
#include "./dispatch_event.hpp"

#include <utility>

namespace Texpainter::Ui
{
	class Listbox
	{
	public:
		explicit Listbox(Container& parent);

		~Listbox();

		Listbox& operator=(Listbox&& obj) noexcept
		{
			std::swap(obj.m_impl, m_impl);
			return *this;
		}

		Listbox(Listbox&& obj) noexcept: m_impl(obj.m_impl) { obj.m_impl = nullptr; }

		Listbox& append(const char* option);

		Listbox& selected(int index) noexcept;

		int selected() const noexcept;

		char const* get(int index) const noexcept;

		Listbox& scrollIntoView(int row) noexcept;

		template<auto id, class EventHandler>
		Listbox& eventHandler(EventHandler& eh)
		{
			return eventHandler(&eh, [](void* event_handler, Listbox& self, int index) {
				auto& obj = *reinterpret_cast<EventHandler*>(event_handler);
				dispatchEvent<id>(
				    [](EventHandler& eh, auto&&... args) {
					    eh.template onActivated<id>(std::forward<decltype(args)>(args)...);
				    },
				    obj,
				    self,
				    index);
			});
		}

		Texpainter::Ui::Listbox& clear() noexcept;

		Texpainter::Ui::Listbox& update() noexcept;

	private:
		using EventHandlerFunc = void (*)(void* event_handler, Listbox& self, int index);
		Listbox& eventHandler(void* event_handler, EventHandlerFunc f);
		class Impl;
		Impl* m_impl;
		Listbox(Impl& impl): m_impl(&impl) {}
	};
}

#endif