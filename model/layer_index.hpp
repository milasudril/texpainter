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

		LayerIndex():m_value{std::numeric_limits<uint32_t>::max()}{}

		explicit LayerIndex(uint32_t val): m_value{val}
		{
		}

		uint32_t value() const
		{
			return m_value;
		}

		auto operator<=>(LayerIndex const&) const = default;

		bool valid() const
		{ return m_value != std::numeric_limits<uint32_t>::max(); }

	private:
		uint32_t m_value;
	};
}

#endif