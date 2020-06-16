//@	{
//@	 "targets":[{"name":"text_entry.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"text_entry.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_TEXTENTRY_HPP
#define TEXPAINTER_UI_TEXTENTRY_HPP

#include "./container.hpp"

#include <utility>

namespace Texpainter::Ui
{
	class TextEntry
	{
	public:
		explicit TextEntry(Container& container);
		~TextEntry();

		TextEntry& operator=(TextEntry&& obj) noexcept
		{
			std::swap(obj.m_impl, m_impl);
			return *this;
		}

		TextEntry(TextEntry&& obj) noexcept: m_impl(obj.m_impl)
		{
			obj.m_impl = nullptr;
		}

		template<auto id, class EventHandler>
		TextEntry& eventHandler(EventHandler& eh)
		{
			return eventHandler(&eh, [](void* event_handler, TextEntry& self) {
				auto& obj = *reinterpret_cast<EventHandler*>(event_handler);
				obj.template onChanged<id>(self);
			});
		}

		const char* content() const noexcept;

		TextEntry& content(const char* text);

		TextEntry& width(int n) noexcept;

		TextEntry& small(bool status) noexcept;

		TextEntry& alignment(float x) noexcept;

		TextEntry& focus();

		TextEntry& enabled(bool status) noexcept;

	protected:
		using EventHandlerFunc = void (*)(void* event_handler, TextEntry& self);
		TextEntry& eventHandler(void* event_handler, EventHandlerFunc f);

		class Impl;
		Impl* m_impl;
		explicit TextEntry(Impl& impl): m_impl(&impl)
		{
		}
	};
}

#endif