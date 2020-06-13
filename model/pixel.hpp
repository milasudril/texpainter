//@	{"targets":[{"name":"pixel.hpp", "type":"include"}]}

#ifndef TEXPAINTER_MODEL_PIXEL_HPP
#define TEXPAINTER_MODEL_PIXEL_HPP

#include "utils/empty.hpp"
#include "utils/vec4.hpp"

#include <ranges>
#include <span>
#include <memory>

namespace Texpainter::Model
{
	class Pixel
	{
	public:
		constexpr explicit Pixel(float r, float g, float b, float a = 1.0f):
			m_value{r,g,b,a}{}

		constexpr Pixel() = default;

		constexpr Pixel& red(float val)
		{
			return *this;
		}

		constexpr Pixel& green(float val)
		{
			return *this;
		}

		constexpr Pixel& blue(float val)
		{
			return *this;
		}

		constexpr Pixel& alpha(float val)
		{
			return *this;
		}



		constexpr float red() const
		{
			return m_value[0];
		}

		constexpr float green() const
		{
			return m_value[1];
		}

		constexpr float blue() const
		{
			return m_value[2];
		}

		constexpr float alpha() const
		{
			return m_value[3];
		}



		constexpr Pixel& operator+=(Pixel other)
		{
			m_value += other.m_value;
			return *this;
		}

		constexpr Pixel& operator-=(Pixel other)
		{
			m_value -= other.m_value;
			return *this;
		}

		constexpr Pixel& operator/=(Pixel other)
		{
			m_value /= other.m_value;
			return *this;
		}

		constexpr Pixel& operator*=(Pixel other)
		{
			m_value *= other.m_value;
			return *this;
		}

		constexpr Pixel& operator*=(float factor)
		{
			m_value *= factor;
			return *this;
		}

		constexpr Pixel& operator/=(float factor)
		{
			m_value /= factor;
			return *this;
		}

	private:
		vec4_t m_value;
	};

	constexpr Pixel red()
	{ return Pixel{1.0f, 0.0f, 0.0f};}

	constexpr Pixel green()
	{ return Pixel{0.0f, 1.0f, 0.0f};}

	constexpr Pixel blue()
	{ return Pixel{0.0f, 0.0f, 1.0f};}

	constexpr Pixel black()
	{ return Pixel{0.0f, 0.0f, 0.0f};}

	constexpr Pixel white()
	{ return Pixel{1.0f, 1.0f, 1.0f};}

	constexpr bool operator==(Pixel a, Pixel b)
	{
		a-=b;
		return a.red() == 0.0f && a.green() == 0.0f && a.blue() == 0.0f && a.alpha() == 0.0f;
	}

	constexpr bool operator!=(Pixel a, Pixel b)
	{
		return !(a == b);
	}

	template<std::ranges::contiguous_range Range, class OutputStream>
	requires std::is_same_v<typename Range::value_type, Pixel>
	void write(Range pixels, OutputStream stream)
	{
		write(stream, std::span{reinterpret_cast<float const*>(std::ranges::data(pixels)), 4*std::ranges::size(pixels)});
	}

	template<class SizedArray, class InputStream>
	SizedArray read(Empty<SizedArray>, InputStream stream)
	{
		auto data = read(stream, Empty<float[]>{});
		using DataBlock = std::decay_t<decltype(data)>;
		auto const size =data.size()/4;
		auto const ptr = reinterpret_cast<Pixel*>(data.release());
		return SizedArray{typename DataBlock::pointer_wrapper<Pixel>{ptr}, size};
	}
}
#endif