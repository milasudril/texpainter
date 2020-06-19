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

		template<class U>
		constexpr explicit(!std::is_same_v<std::decay_t<U>, std::decay_t<T>>) Span2d(Span2d<U> other):
		   r_ptr{other.data()},
		   m_width{other.width()},
		   m_height{other.height()}
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

		std::tuple<uint32_t, uint32_t> size() const
		{
			return {m_width, m_height};
		}

	private:
		T* r_ptr;
		uint32_t m_width;
		uint32_t m_height;
	};

	template<class T, class Func>
	void for_each(Span2d<T> span, Func&& f)
	{
		for(uint32_t row = 0; row < span.height(); ++row)
		{
			for(uint32_t col = 0; col < span.width(); ++col)
			{
				f(col, row, span(col, row));
			}
		}
	}

	template<class T, class U, class Func>
	void transform(Span2d<T> in, Span2d<U> out, Func&& f)
	{
		for(uint32_t row = 0; row < in.height(); ++row)
		{
			for(uint32_t col = 0; col < in.width(); ++col)
			{
				out(col, row) = f(col, row, in(col, row));
			}
		}
	}

	template<class T, class Func>
	void generate(Span2d<T> out, Func&& f)
	{
		for(uint32_t row = 0; row < out.height(); ++row)
		{
			for(uint32_t col = 0; col < out.width(); ++col)
			{
				out(col, row) = f(col, row);
			}
		}
	}
}

#endif