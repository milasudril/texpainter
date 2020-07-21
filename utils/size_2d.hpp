//@	{"targets":[{"name":"size_2d.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_SIZE2D_HPP
#define TEXPAINTER_UTILS_SIZE2D_HPP

#include "./empty.hpp"

#include <cstdint>
#include <limits>

namespace Texpainter
{
	class Size2d
	{
	public:
		constexpr explicit Size2d(uint32_t width, uint32_t height): m_width{width}, m_height{height}
		{
		}

		constexpr auto width() const { return m_width; }

		constexpr auto height() const { return m_height; }

		constexpr auto area() const
		{
			return static_cast<size_t>(m_width) * static_cast<size_t>(m_height);
		}

	private:
		uint32_t m_width;
		uint32_t m_height;
	};

	constexpr inline bool operator==(Size2d a, Size2d b)
	{
		return a.width() == b.width() && a.width() == b.height();
	}

	constexpr inline bool operator!=(Size2d a, Size2d b) { return !(a == b); }

	template<class OutputStream>
	void write(Size2d size, OutputStream stream)
	{
		write(size.width(), stream);
		write(size.height(), stream);
	}

	template<class InputStream>
	Size2d read(Empty<Size2d>, InputStream stream)
	{
		auto const w = read(Empty<uint32_t>{}, stream);
		auto const h = read(Empty<uint32_t>{}, stream);
		return Size2d{w, h};
	}

	template<class T>
	bool isSupported(Size2d size)
	{
		auto const a = size.area();
		return a != 0 && a < std::numeric_limits<size_t>::max() / sizeof(T);
	}
}

#endif