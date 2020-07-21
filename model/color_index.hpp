//@	{"targets":[{"name":"color_index.hpp", "type":"include"}]}

#ifndef TEXPAINTER_COLORINDEX_HPP
#define TEXPAINTER_COLORINDEX_HPP

#include <cstdint>

namespace Texpainter::Model
{
	class ColorIndex
	{
	public:
		using element_type = uint32_t;

		ColorIndex(): m_value{std::numeric_limits<uint32_t>::max()} {}

		explicit ColorIndex(uint32_t val): m_value{val} {}

		uint32_t value() const { return m_value; }

		auto operator<=>(ColorIndex const&) const = default;

		bool valid() const { return m_value != std::numeric_limits<uint32_t>::max(); }

		ColorIndex& operator++()
		{
			++m_value;
			return *this;
		}

		ColorIndex& operator--()
		{
			--m_value;
			return *this;
		}

	private:
		uint32_t m_value;
	};
}

#endif