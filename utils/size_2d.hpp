//@	{"targets":[{"name":"size_2d.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_SIZE2D_HPP
#define TEXPAINTER_UTILS_SIZE2D_HPP

#include <cstdint>
#include <limits>
#include <string>

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
		return a.width() == b.width() && a.height() == b.height();
	}

	constexpr inline bool operator!=(Size2d a, Size2d b) { return !(a == b); }

	template<class T>
	bool isSupported(Size2d size)
	{
		auto const a = size.area();
		return a != 0 && a < std::numeric_limits<size_t>::max() / sizeof(T);
	}

	inline std::string toString(Size2d size)
	{
		return std::to_string(size.width()) + " × " + std::to_string(size.height());
	}
}

#endif