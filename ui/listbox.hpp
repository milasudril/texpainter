//@	{
//@	"targets":[{"name":"listbox.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"listbox.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_LISTBOX_HPP
#define TEXPAINTER_UI_LISTBOX_HPP

#include "./container.hpp"

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

		Listbox& clear() noexcept;

		template<auto id, class EventHandler>
		Listbox& eventHandler(EventHandler& eh)
		{
			return eventHandler(&eh, [](void* event_handler, Listbox& self, int index) {
				auto& obj = *reinterpret_cast<EventHandler*>(event_handler);
				obj.template onActiveted<id>(self, index);
			});
		}

	private:
		using EventHandlerFunc = void (*)(void* event_handler, Listbox& self, int index);
		Listbox& eventHandler(void* event_handler, EventHandlerFunc f);
		class Impl;
		Impl* m_impl;
		Listbox(Impl& impl): m_impl(&impl) {}
	};
}

#endif