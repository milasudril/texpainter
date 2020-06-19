//@	{"targets":[{"name":"size_2d.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_SIZE2D_HPP
#define TEXPAINTER_UTILS_SIZE2D_HPP

#include <cstdint>

namespace Texpainter
{
	class Size2d
	{
		public:
			constexpr explicit Size2d(uint32_t width, uint32_t height): m_width{width}, m_height{height}
			{}

			constexpr auto width() const
			{ return m_width; }

			constexpr auto height() const
			{ return m_height; }

			constexpr auto area() const
			{ return static_cast<size_t>(m_width) * static_cast<size_t>(m_height);}

		private:
			uint32_t m_width;
			uint32_t m_height;
	};

	constexpr inline bool operator==(Size2d a, Size2d b)
	{ return a.width() == b.width() && a.width() == b.height();}

	constexpr inline bool operator!=(Size2d a, Size2d b)
	{ return !(a == b);}
}

#endif