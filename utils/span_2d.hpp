//@	{"targets":[{"name":"span_2d.hpp", "type":"include"}]}

#ifndef TEXPAINTER_SPAN_2D_HPP
#define TEXPAINTER_SPAN_2D_HPP

#include "./size_2d.hpp"

namespace Texpainter
{
	template<class T>
	class Span2d
	{
	public:
		constexpr explicit Span2d(T* ptr, uint32_t w, uint32_t h): Span2d{ptr, Size2d{w, h}} {}

		constexpr Span2d(T* ptr, Size2d size): r_ptr{ptr}, m_size{size} {}

		template<class U>
		constexpr explicit(!std::is_same_v<std::decay_t<U>, std::decay_t<T>>)
		    Span2d(Span2d<U> other)
		    : r_ptr{other.data()}
		    , m_size{other.size()}
		{
		}

		constexpr auto begin() const { return r_ptr; }

		constexpr auto begin() { return r_ptr; }

		constexpr auto end() const { return r_ptr + area(); }

		constexpr auto end() { return r_ptr + area(); }

		constexpr auto data() const { return begin(); }

		constexpr auto data() { return begin(); }

		constexpr auto width() const { return m_size.width(); }

		constexpr auto height() const { return m_size.height(); }

		constexpr auto area() const { return m_size.area(); }

		constexpr T& operator()(uint32_t x, uint32_t y)
		{
			auto ptr = begin();
			return *(ptr + y * width() + x);
		}

		constexpr T const& operator()(uint32_t x, uint32_t y) const
		{
			auto ptr = begin();
			return *(ptr + y * width() + x);
		}

		constexpr auto size() const { return m_size; }

	private:
		T* r_ptr;
		Size2d m_size;
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