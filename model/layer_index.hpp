//@	{"targets":[{"name":"layer_index.hpp", "type":"include"}]}

#ifndef TEXPAINTER_MODEL_LAYERINDEX_HPP
#define TEXPAINTER_MODEL_LAYERINDEX_HPP

#include <cstdint>
#include <compare>

namespace Texpainter::Model
{
	class LayerIndex
	{
	public:
		using element_type = uint32_t;

		constexpr LayerIndex(): m_value{std::numeric_limits<uint32_t>::max()} {}

		constexpr explicit LayerIndex(size_t val): m_value{static_cast<element_type>(val)} {}


		constexpr element_type value() const { return m_value; }

		constexpr auto operator<=>(LayerIndex const&) const = default;

		constexpr bool valid() const { return m_value != std::numeric_limits<element_type>::max(); }

		constexpr LayerIndex& operator++()
		{
			++m_value;
			return *this;
		}

		constexpr LayerIndex& operator--()
		{
			--m_value;
			return *this;
		}

		constexpr explicit operator size_t() const { return m_value; }

		constexpr explicit operator int() const { return static_cast<int>(m_value); }

	private:
		element_type m_value;
	};
}

#endif