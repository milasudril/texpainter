//@	{"targets":[{"name":"color_index.hpp", "type":"include"}]}

#ifndef TEXPAINTER_PIXELSTORE_COLORINDEX_HPP
#define TEXPAINTER_PIXELSTORE_COLORINDEX_HPP

#include <cstdint>
#include <compare>
#include <concepts>

namespace Texpainter::PixelStore
{
	class ColorIndex
	{
	public:
		using element_type = uint8_t;

		constexpr ColorIndex(): m_value{std::numeric_limits<element_type>::max()} {}

		constexpr explicit ColorIndex(element_type val): m_value{val} {}

		constexpr element_type value() const { return m_value; }

		constexpr auto operator<=>(ColorIndex const&) const = default;

		constexpr bool valid() const { return m_value != std::numeric_limits<element_type>::max(); }

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
		element_type m_value;
	};

	inline void to_json(nlohmann::json& obj, ColorIndex id) { obj = nlohmann::json{id.value()}; }
}

#endif