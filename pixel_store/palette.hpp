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
		auto begin() const { return std::begin(m_data); }

		auto end() const { return std::end(m_data); }

		auto begin() { return std::begin(m_data); }

		auto end() { return std::end(m_data); }

		auto size() const { return std::size(m_data); }


		explicit Palette(Pixel color_init = Pixel{0.0f, 0.0f, 0.0f, 0.0f})
		{
			std::ranges::fill(m_data, color_init);
		}

		auto operator[](ColorIndex index) const { return *(begin() + index.value()); }

		auto& operator[](ColorIndex index) { return *(std::begin(m_data) + index.value()); }

		auto lastIndex() const { return ColorIndex{size() - 1}; }

	private:
		std::array<Pixel, Size> m_data;
	};
}

#endif