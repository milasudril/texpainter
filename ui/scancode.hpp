//@	{
//@	"targets":[{"name":"scancode.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UI_SCANCODE_HPP
#define TEXPAINTER_UI_SCANCODE_HPP

#include <cstdint>
#include <compare>

namespace Texpainter::Ui
{
	class Scancode
	{
	public:
		using element_type = uint8_t;

		constexpr explicit Scancode(uint8_t val): m_value{val} {}

		constexpr auto operator<=>(Scancode const&) const = default;

		constexpr auto value() const { return m_value; }

	private:
		uint8_t m_value;
	};

	namespace Scancodes
	{
		constexpr Scancode Return{28};
		constexpr Scancode ReturnNumpad{96};
		constexpr Scancode Escape{1};
		constexpr Scancode ShiftLeft{42};
		constexpr Scancode ShiftRight{54};
		constexpr Scancode AltLeft{56};
		constexpr Scancode A{30};
	}
}

#endif