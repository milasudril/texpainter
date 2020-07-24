//@	{
//@  "targets":[{"name":"slider.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"slider.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_SLIDER_HPP
#define TEXPAINTER_UI_SLIDER_HPP

#include "./container.hpp"

#include <utility>
#include <algorithm>
#include <cmath>
#include <span>

namespace Texpainter::Ui
{
	class SliderValue
	{
	public:
		constexpr SliderValue() = default;

		constexpr explicit SliderValue(double val): m_value{val} {}

		// SIGSEGV during compilation in gcc 10.1 [bug 96064]
		// constexpr bool operator<=>(SliderValue const&) const = default;

		constexpr double value() const { return m_value; }

	private:
		double m_value;
	};

	inline constexpr SliderValue logValue(double x,
	                                      double min_exponent = -10.0,
	                                      double max_exponent = 0.0)
	{
		x              = (x == 0.0) ? std::exp(min_exponent - 1.0) : x;
		auto const ret = (std::log2(x) - min_exponent) / (max_exponent - min_exponent);
		return Texpainter::Ui::SliderValue{ret};
	}

	inline constexpr double logValue(SliderValue val,
	                                 double min_exponent = -10.0,
	                                 double max_exponent = 0.0)
	{
		auto const x = val.value();

		return x == 0.0 ? 0.0 : std::exp2(min_exponent * (1.0 - x) + x * max_exponent);
	}

	inline constexpr SliderValue linValue(double x, double min = 0.0, double max = 1.0)
	{
		return Texpainter::Ui::SliderValue{(x - min) / (max - min)};
	}

	inline constexpr double linValue(SliderValue val, double min = 0.0, double max = 1.0)
	{
		auto const x = val.value();
		return min * (1.0 - x) + max * x;
	}

	class Slider
	{
	public:
		struct TickMark
		{
			SliderValue location;
			char const* text;
		};

		explicit Slider(Container& container, bool vertical);
		~Slider();

		Slider& operator=(Slider&& obj) noexcept
		{
			std::swap(obj.m_impl, m_impl);
			return *this;
		}

		Slider(Slider&& obj) noexcept: m_impl(obj.m_impl) { obj.m_impl = nullptr; }

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

		Slider& ticks(std::span<TickMark const> marks);

	protected:
		using EventHandlerFunc = void (*)(void* event_handler, Slider& self);
		Slider& eventHandler(void* event_handler, EventHandlerFunc f);

		class Impl;
		Impl* m_impl;
		explicit Slider(Impl& impl): m_impl(&impl) {}
	};
}

#endif
