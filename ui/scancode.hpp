//@	{
//@	"targets":[{"name":"scancode.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UI_SCANCODE_HPP
#define TEXPAINTER_UI_SCANCODE_HPP

#include <cstdint>

namespace Texpainter::Ui
{
	class Scancode
	{
	public:
		explicit Scancode(uint8_t val): m_value{val} {}

		auto operator<=>(Scancode const&) const = default;

		auto value() const { return m_value; }

	private:
		uint8_t m_value;
	};
}

#endif