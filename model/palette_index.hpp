//@	{"targets":[{"name":"palette_index.hpp", "type":"include"}]}

#ifndef TEXPAINTER_PALETTEINDEX_HPP
#define TEXPAINTER_PALETTEINDEX_HPP

#include <cstdint>

namespace Texpainter::Model
{
	class PaletteIndex
	{
	public:
		using element_type = uint32_t;

		PaletteIndex(): m_value{std::numeric_limits<uint32_t>::max()} {}

		explicit PaletteIndex(uint32_t val): m_value{val} {}

		uint32_t value() const { return m_value; }

		auto operator<=>(PaletteIndex const&) const = default;

		bool valid() const { return m_value != std::numeric_limits<uint32_t>::max(); }

		PaletteIndex& operator++()
		{
			++m_value;
			return *this;
		}

		PaletteIndex& operator--()
		{
			--m_value;
			return *this;
		}

	private:
		uint32_t m_value;
	};
}

#endif