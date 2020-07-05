//@	{
//@  "targets":[{"name":"slider.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"slider.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_SLIDER_HPP
#define TEXPAINTER_UI_SLIDER_HPP

#include "./container.hpp"

#include <utility>

namespace Texpainter::Ui
{
	class SliderValue
	{
	public:
		explicit SliderValue(double val): m_value{val}
		{
		}

		bool operator<=>(SliderValue const&) const = default;

		double value() const
		{
			return m_value;
		}

	private:
		double m_value;
	};

	class Slider
	{
	public:
		explicit Slider(Container& container, bool vertical);
		~Slider();

		Slider& operator=(Slider&& obj) noexcept
		{
			std::swap(obj.m_impl, m_impl);
			return *this;
		}

		Slider(Slider&& obj) noexcept: m_impl(obj.m_impl)
		{
			obj.m_impl = nullptr;
		}

		template<auto id, class EventHandler>
		Slider& eventHandler(EventHandler& eh)
		{
			return eventHandler(&eh, [](void* event_handler, Slider& self) {
				auto& obj = *reinterpret_cast<EventHandler*>(event_handler);
				obj.template onChanged<id>(self);
			});
		}

		SliderValue value() const noexcept;

		Slider& value(SliderValue x);

	protected:
		using EventHandlerFunc = void (*)(void* event_handler, Slider& self);
		Slider& eventHandler(void* event_handler, EventHandlerFunc f);

		class Impl;
		Impl* m_impl;
		explicit Slider(Impl& impl): m_impl(&impl)
		{
		}
	};
}

#endif
