//@	{
//@	"targets":[{"name":"combobox.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"combobox.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_COMBOBOX_HPP
#define TEXPAINTER_UI_COMBOBOX_HPP

#include "./container.hpp"
#include "./dispatch_event.hpp"

#include <utility>

namespace Texpainter::Ui
{
	class Combobox
	{
	public:
		explicit Combobox(Container& parent);

		explicit Combobox(Container& parent, char const* first_item): Combobox{parent}
		{
			append(first_item);
		}

		~Combobox();

		Combobox& operator=(Combobox&& obj) noexcept
		{
			std::swap(obj.m_impl, m_impl);
			return *this;
		}

		Combobox(Combobox&& obj) noexcept: m_impl(obj.m_impl) { obj.m_impl = nullptr; }

		Combobox& append(const char* option);

		Combobox& replace(int index, const char* option);

		Combobox& selected(int index) noexcept;

		int selected() const noexcept;

		Combobox& clear() noexcept;

		template<auto id, class EventHandler>
		Combobox& eventHandler(EventHandler& eh)
		{
			return eventHandler(&eh, [](void* event_handler, Combobox& self) {
				auto& obj = *reinterpret_cast<EventHandler*>(event_handler);
				dispatchEvent<id>(
				    [](EventHandler& eh, auto&&... args) {
					    eh.template onChanged<id>(std::forward<decltype(args)>(args)...);
				    },
				    obj,
				    self);
			});
		}

	private:
		using EventHandlerFunc = void (*)(void* event_handler, Combobox& self);
		Combobox& eventHandler(void* event_handler, EventHandlerFunc f);
		class Impl;
		Impl* m_impl;
		Combobox(Impl& impl): m_impl(&impl) {}
	};
}

#endif