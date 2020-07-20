//@	{"targets":[{"name":"layer_index.hpp", "type":"include"}]}

#ifndef TEXPAINTER_LAYERINDEX_HPP
#define TEXPAINTER_LAYERINDEX_HPP

#include <cstdint>

namespace Texpainter::Model
{
	class LayerIndex
	{
	public:
		using element_type = uint32_t;

		LayerIndex(): m_value{std::numeric_limits<uint32_t>::max()}
		{
		}

		explicit LayerIndex(size_t val): m_value{static_cast<element_type>(val)}
		{
		}


		element_type value() const
		{
			return m_value;
		}

		auto operator<=>(LayerIndex const&) const = default;

		bool valid() const
		{
			return m_value != std::numeric_limits<element_type>::max();
		}

		LayerIndex& operator++()
		{
			++m_value;
			return *this;
		}

		LayerIndex& operator--()
		{
			--m_value;
			return *this;
		}

		explicit operator size_t() const
		{
			return m_value;
		}

		explicit operator int() const
		{
			return static_cast<int>(m_value);
		}

	private:
		element_type m_value;
	};
}

#endif