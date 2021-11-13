//@	{"targets":[{"name":"span_2d.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_SPAN2D_HPP
#define TEXPAINTER_UTILS_SPAN2D_HPP

#include "./size_2d.hpp"

namespace Texpainter
{
	template<class T>
	class Span2d
	{
	public:
		constexpr explicit Span2d(): Span2d{nullptr, Size2d{0, 0}} {}

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

		constexpr auto end() const { return r_ptr + area(m_size); }

		constexpr auto end() { return r_ptr + area(m_size); }

		constexpr auto data() const { return begin(); }

		constexpr auto data() { return begin(); }

		constexpr auto width() const { return m_size.width(); }

		constexpr auto height() const { return m_size.height(); }

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

	template<class T>
	constexpr inline auto area(Span2d<T> s)
	{
		return area(s.size());
	}

	template<class T>
	constexpr inline auto aspectRatio(Span2d<T> s)
	{
		return aspectRatio(s.size());
	}

	template<class T, class Func>
	void constexpr for_each(Span2d<T> span, Func&& f)
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
	void constexpr transform(Span2d<T> in, Span2d<U> out, Func&& f)
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
	void constexpr generate(Span2d<T> out, Func&& f)
	{
		for(uint32_t row = 0; row < out.height(); ++row)
		{
			for(uint32_t col = 0; col < out.width(); ++col)
			{
				out(col, row) = f(col, row);
			}
		}
	}

	struct Span2dBoundingBox
	{
		uint32_t x_min;
		uint32_t x_max;
		uint32_t y_min;
		uint32_t y_max;

		constexpr auto width() const { return x_max - x_min; }
		constexpr auto height() const { return y_max - y_min; }
		constexpr bool valid() const { return x_max > x_min && y_max > y_min; }

		constexpr bool operator==(Span2dBoundingBox const&) const = default;
		constexpr bool operator!=(Span2dBoundingBox const&) const = default;
	};

	template<class T, class UnaryPredicate>
	Span2dBoundingBox boundingBox(Span2d<T const> data, UnaryPredicate pred)
	{
		uint32_t x_min = data.width();
		uint32_t x_max = 0;
		uint32_t y_min = data.height();
		uint32_t y_max = 0;

		for(uint32_t row = 0; row < data.height(); ++row)
		{
			for(uint32_t col = 0; col < data.width(); ++col)
			{
				if(pred(data(col, row)))
				{
					x_min = std::min(x_min, col);
					x_max = std::max(x_max, col + 1);
					y_min = std::min(y_min, row);
					y_max = std::max(y_max, row + 1);
				}
			}
		}

		return Span2dBoundingBox{x_min, x_max, y_min, y_max};
	}
}

#endif