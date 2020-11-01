//@	{"targets":[{"name":"palette.hpp", "type":"include"}]}

#ifndef TEXPAINTER_PIXELSTORE_PALETTE_HPP
#define TEXPAINTER_PIXELSTORE_PALETTE_HPP

#include "./pixel.hpp"
#include "./color_index.hpp"

#include <array>
#include <algorithm>

namespace Texpainter::PixelStore
{
	template<size_t Size>
	class Palette
	{
	public:
		using index_type = ColorIndex;

		constexpr auto begin() const { return std::begin(m_data); }

		constexpr auto end() const { return std::end(m_data); }

		constexpr auto begin() { return std::begin(m_data); }

		constexpr auto end() { return std::end(m_data); }

		static constexpr auto size() { return Size; }


		constexpr explicit Palette(Pixel color_init = Pixel{0.0f, 0.0f, 0.0f, 0.0f})
		{
			std::ranges::fill(m_data, color_init);
		}

		constexpr auto operator[](index_type index) const { return *(begin() + index.value()); }

		constexpr auto& operator[](index_type index)
		{
			return *(std::begin(m_data) + index.value());
		}

		constexpr auto lastIndex() const { return index_type{size() - 1}; }

	private:
		std::array<Pixel, Size> m_data;
	};
}

#endif