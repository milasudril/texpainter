//@	{"targets":[{"name":"palette_index.hpp", "type":"include"}]}

#ifndef TEXPAINTER_PALETTEINDEX_HPP
#define TEXPAINTER_PALETTEINDEX_HPP

#include <cstdint>
#include <compare>

namespace Texpainter::Model
{
	class PaletteIndex
	{
	public:
		using element_type = uint32_t;

		constexpr PaletteIndex(): m_value{std::numeric_limits<uint32_t>::max()} {}

		constexpr explicit PaletteIndex(uint32_t val): m_value{val} {}

		constexpr uint32_t value() const { return m_value; }

		constexpr auto operator<=>(PaletteIndex const&) const = default;

		constexpr bool valid() const { return m_value != std::numeric_limits<uint32_t>::max(); }

		constexpr PaletteIndex& operator++()
		{
			++m_value;
			return *this;
		}

		constexpr PaletteIndex& operator--()
		{
			--m_value;
			return *this;
		}

		constexpr explicit operator size_t() const { return m_value; }

	private:
		uint32_t m_value;
	};
}

#endif