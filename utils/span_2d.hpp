//@	{"targets":[{"name":"span_2d.hpp", "type":"include"}]}

#ifndef TEXPAINTER_SPAN_2D_HPP
#define TEXPAINTER_SPAN_2D_HPP

#include <cstdint>

namespace Texpainter
{
	template<class T>
	class Span2d
	{
	public:
		constexpr Span2d(T* ptr, uint32_t width, uint32_t height):
		   r_ptr{ptr},
		   m_width{width},
		   m_height{height}
		{
		}

		constexpr auto begin() const
		{
			return r_ptr;
		}

		constexpr auto begin()
		{
			return r_ptr;
		}

		constexpr auto end() const
		{
			return r_ptr + area();
		}

		constexpr auto end()
		{
			return r_ptr + area();
		}

		constexpr auto data() const
		{
			return begin();
		}

		constexpr auto data()
		{
			return begin();
		}

		auto width() const
		{
			return m_width;
		}

		auto height() const
		{
			return m_height;
		}

		size_t area() const
		{
			return static_cast<size_t>(m_width) * static_cast<size_t>(m_height);
		}

		T& operator()(uint32_t x, uint32_t y)
		{
			auto ptr = begin();
			return *(ptr + y * width() + x);
		}

		T const& operator()(uint32_t x, uint32_t y) const
		{
			auto ptr = begin();
			return *(ptr + y * width() + x);
		}

	private:
		T* r_ptr;
		uint32_t m_width;
		uint32_t m_height;
	};
}

#endif