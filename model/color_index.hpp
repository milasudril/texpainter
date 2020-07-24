//@	{"targets":[{"name":"color_index.hpp", "type":"include"}]}

#ifndef TEXPAINTER_COLORINDEX_HPP
#define TEXPAINTER_COLORINDEX_HPP

#include <cstdint>
#include <compare>

namespace Texpainter::Model
{
	class ColorIndex
	{
	public:
		using element_type = uint32_t;

		constexpr ColorIndex(): m_value{std::numeric_limits<uint32_t>::max()} {}

		constexpr explicit ColorIndex(uint32_t val): m_value{val} {}

		constexpr uint32_t value() const { return m_value; }

		constexpr auto operator<=>(ColorIndex const&) const = default;

		constexpr bool valid() const { return m_value != std::numeric_limits<uint32_t>::max(); }

		constexpr ColorIndex& operator++()
		{
			++m_value;
			return *this;
		}

		constexpr ColorIndex& operator--()
		{
			--m_value;
			return *this;
		}

	private:
		uint32_t m_value;
	};
}

#endif